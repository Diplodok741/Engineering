#!/usr/bin/env python3
"""
wch.py — аналог idf.py для CH32 (WCH)
Поддерживает запуск из папки проекта (Project/xxx)
"""

import typer
import subprocess
import shutil
import time
import serial
import serial.tools.list_ports
from pathlib import Path
from typing import Optional
from rich.console import Console
from rich.panel import Panel

console = Console()
app = typer.Typer(
    help="🔧 wch.py — утилита сборки для CH32",
    add_completion=True,
    no_args_is_help=True,
)


def find_project_root() -> Path:
    """Ищет корень репозитория (где лежит CMakeLists.txt и wch.py)"""
    current = Path.cwd().resolve()
    while current != current.parent:
        if (current / "CMakeLists.txt").exists() and (current / "wch.py").exists():
            return current
        current = current.parent
    console.print("[bold red]❌ Не найден корень проекта (CMakeLists.txt + wch.py)![/]")
    raise typer.Exit(1)


def get_current_project_dir() -> Path:
    """Определяет папку текущего проекта (откуда запущена команда)"""
    cwd = Path.cwd().resolve()
    root = find_project_root()

    # Если мы уже внутри Project/xxx — используем эту папку
    if cwd.is_relative_to(root / "Project"):
        return cwd
    else:
        # Если запущено из корня — используем по умолчанию blink
        default = root / "Project" / "blink"
        if default.exists():
            console.print(f"[yellow]Предупреждение: Запущено из корня. Используется проект по умолчанию: blink[/]")
            return default
        else:
            console.print("[bold red]❌ Папка Project/blink не найдена![/]")
            raise typer.Exit(1)


def ensure_build_dir(project_dir: Path) -> Path:
    """Создаёт build/ внутри папки проекта"""
    build_dir = project_dir / "build"
    build_dir.mkdir(exist_ok=True)
    return build_dir


def run_cmake_configure(project_dir: Path, root: Path, verbose: bool = False):
    """Конфигурация CMake"""
    build_dir = ensure_build_dir(project_dir)

    console.print(Panel.fit(f"[cyan]Конфигурация CMake для проекта: [bold]{project_dir.name}[/][/]", 
                           border_style="cyan"))

    cmd = [
        "cmake",
        "-B", str(build_dir),
        "-S", str(root),                    # Главный CMakeLists.txt всегда в корне
        "-G", "Ninja",
        "-DCMAKE_BUILD_TYPE=Release",
    ]

    if verbose:
        cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")

    try:
        subprocess.run(cmd, cwd=root, check=True, capture_output=False)
    except subprocess.CalledProcessError:
        console.print("[bold red]❌ Ошибка конфигурации CMake[/]")
        raise typer.Exit(1)


def build_firmware(root: Path, verbose: bool = False):
    project_dir = get_current_project_dir()
    run_cmake_configure(project_dir, root, verbose)

    build_dir = project_dir / "build"

    console.print(Panel.fit("[cyan]Выполняется сборка...[/]", border_style="cyan"))

    cmd = ["cmake", "--build", str(build_dir)]
    if verbose:
        cmd.append("--verbose")

    try:
        subprocess.run(cmd, cwd=root, check=True)
        console.print("[bold green]✅ Сборка завершена успешно![/]")
        show_size(build_dir, root)
    except subprocess.CalledProcessError:
        console.print("[bold red]❌ Ошибка сборки![/]")
        raise typer.Exit(1)


def show_size(build_dir: Path, root: Path):
    console.print(Panel.fit("[cyan]Размер прошивки:[/]", border_style="cyan"))
    subprocess.run(
        ["cmake", "--build", str(build_dir), "--target", "size"],
        cwd=root,
        check=True,
    )


# ====================== КОМАНДЫ ======================

@app.command()
def build(verbose: bool = typer.Option(False, "--verbose", "-v", help="Подробный вывод")):
    """Собрать текущий проект"""
    root = find_project_root()
    build_firmware(root, verbose)


@app.command()
def flash(verbose: bool = typer.Option(False, "--verbose", "-v", help="Подробный вывод")):
    """Собрать и прошить"""
    root = find_project_root()
    build_firmware(root, verbose)

    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"

    console.print(Panel.fit("[yellow]Прошиваем через wlink...[/]", border_style="yellow"))
    try:
        subprocess.run(
            ["cmake", "--build", str(build_dir), "--target", "flash"],
            cwd=root,
            check=True,
        )
        console.print("[bold green]✅ Прошивка завершена![/]")
    except subprocess.CalledProcessError:
        console.print("[bold red]❌ Ошибка прошивки![/]")


@app.command()
def clean():
    """Очистить build в текущей папке проекта"""
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
    if build_dir.exists():
        shutil.rmtree(build_dir)
        console.print(f"[green]✅ build очищен в {project_dir.name}[/]")
    else:
        console.print("[yellow]build уже отсутствует[/]")


@app.command()
def size():
    """Показать размер прошивки"""
    root = find_project_root()
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
    if not build_dir.exists():
        console.print("[red]Сначала выполните сборку (build)[/]")
        raise typer.Exit(1)
    show_size(build_dir, root)


@app.command()
def erase():
    """Стереть чип"""
    root = find_project_root()
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
    console.print(Panel.fit("[red]Стираем чип...[/]", border_style="red"))
    subprocess.run(
        ["cmake", "--build", str(build_dir), "--target", "erase"],
        cwd=root,
        check=True,
    )
    console.print("[bold green]✅ Чип стёрт![/]")


@app.command()
def monitor(
    port: Optional[str] = typer.Option(None, "--port", "-p"),
    baud: int = typer.Option(115200, "--baud", "-b"),
):
    """Монитор UART"""
    # (тот же код монитора, что был раньше)
    import serial.tools.list_ports
    if port is None:
        ports = [p.device for p in serial.tools.list_ports.comports()
                 if any(x in p.description.lower() for x in ["ch340", "ch341", "wch", "usb"])]
        port = ports[0] if len(ports) == 1 else None

        if not port:
            console.print("[red]Порт не найден. Укажите --port[/]")
            raise typer.Exit(1)

    console.print(Panel.fit(
        f"[green]Монитор запущен на {port} @ {baud} бод[/]\n[dim]Ctrl+C для выхода[/]",
        border_style="green"
    ))

    try:
        ser = serial.Serial(port, baud, timeout=0.1)
        while True:
            if ser.in_waiting:
                line = ser.readline().decode("utf-8", errors="replace").rstrip()
                if line:
                    console.print(line)
            time.sleep(0.01)
    except KeyboardInterrupt:
        console.print("\n[yellow]Монитор остановлен[/]")
    finally:
        if 'ser' in locals():
            ser.close()


if __name__ == "__main__":
    app()



# #!/usr/bin/env python3
# """
# wch.py — аналог idf.py для микроконтроллеров CH32 (WCH)
# """
#
# import typer
# import subprocess
# import shutil
# import time
# import serial
# import serial.tools.list_ports
# from pathlib import Path
# from typing import Optional
# from rich.console import Console
# from rich.panel import Panel
#
# console = Console()
# app = typer.Typer(
#     help="🔧 wch.py — утилита сборки и прошивки для CH32 (как idf.py)",
#     add_completion=True,
#     no_args_is_help=True,
# )
#
#
# def find_project_root() -> Path:
#     """Находит корень проекта по наличию CMakeLists.txt."""
#     current = Path.cwd().resolve()
#     while current != current.parent:
#         if (current / "CMakeLists.txt").exists():
#             return current
#         current = current.parent
#     console.print("[bold red]❌ Не найден корень проекта (CMakeLists.txt)![/]")
#     raise typer.Exit(1)
#
#
# def run_cmake_configure(root: Path, verbose: bool = False):
#     """Конфигурация CMake."""
#     build_dir = root / "build"
#     build_dir.mkdir(exist_ok=True)
#
#     console.print(Panel.fit("[cyan]Конфигурация CMake (Ninja)...[/]", border_style="cyan"))
#
#     cmd = [
#         "cmake",
#         "-B", str(build_dir),
#         "-S", str(root),
#         "-G", "Ninja",
#         "-DCMAKE_BUILD_TYPE=Release",
#     ]
#
#     if verbose:
#         cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")
#
#     try:
#         subprocess.run(cmd, cwd=root, check=True, capture_output=False)
#     except subprocess.CalledProcessError as e:
#         console.print("[bold red]❌ Ошибка конфигурации CMake[/]")
#         raise typer.Exit(1)
#
#
# def build_firmware(root: Path, verbose: bool = False):
#     """Сборка + автоматический показ размера."""
#     run_cmake_configure(root, verbose)
#
#     console.print(Panel.fit("[cyan]Сборка прошивки...[/]", border_style="cyan"))
#
#     cmd = ["cmake", "--build", str(root / "build")]
#     if verbose:
#         cmd.append("--verbose")
#
#     try:
#         subprocess.run(cmd, cwd=root, check=True)
#         console.print("[bold green]✅ Сборка завершена успешно![/]")
#         show_size(root)                    # автоматически показываем размер
#     except subprocess.CalledProcessError:
#         console.print("[bold red]❌ Ошибка сборки![/]")
#         raise typer.Exit(1)
#
#
# def show_size(root: Path):
#     """Показывает размер прошивки."""
#     console.print(Panel.fit("[cyan]Размер прошивки:[/]", border_style="cyan"))
#     subprocess.run(
#         ["cmake", "--build", str(root / "build"), "--target", "size"],
#         cwd=root,
#         check=True,
#     )
#
#
# # ====================== КОМАНДЫ ======================
#
# @app.command()
# def build(verbose: bool = typer.Option(False, "--verbose", "-v", help="Подробный вывод")):
#     """Собрать проект (как idf.py build)."""
#     root = find_project_root()
#     build_firmware(root, verbose)
#
#
# @app.command()
# def flash(verbose: bool = typer.Option(False, "--verbose", "-v", help="Подробный вывод")):
#     """Собрать и прошить (как idf.py flash)."""
#     root = find_project_root()
#     build_firmware(root, verbose)
#
#     console.print(Panel.fit("[yellow]Прошиваем через wlink...[/]", border_style="yellow"))
#     try:
#         subprocess.run(
#             ["cmake", "--build", str(root / "build"), "--target", "flash"],
#             cwd=root,
#             check=True,
#         )
#         console.print("[bold green]✅ Прошивка завершена![/]")
#     except subprocess.CalledProcessError:
#         console.print("[bold red]❌ Ошибка прошивки![/]")
#
#
# @app.command()
# def erase():
#     """Стереть чип (как idf.py erase)."""
#     root = find_project_root()
#     run_cmake_configure(root)
#     console.print(Panel.fit("[red]Стираем чип...[/]", border_style="red"))
#     subprocess.run(
#         ["cmake", "--build", str(root / "build"), "--target", "erase"],
#         cwd=root,
#         check=True,
#     )
#     console.print("[bold green]✅ Чип успешно стёрт![/]")
#
#
# @app.command()
# def size():
#     """Показать размер прошивки."""
#     root = find_project_root()
#     show_size(root)
#
#
# @app.command()
# def clean():
#     """Очистить build-директорию."""
#     root = find_project_root()
#     build_dir = root / "build"
#     if build_dir.exists():
#         shutil.rmtree(build_dir)
#         console.print("[green]✅ Директория build очищена[/]")
#     else:
#         console.print("[yellow]build уже отсутствует[/]")
#
#
# @app.command()
# def fullclean():
#     """Полная очистка (build + ninja кэш)."""
#     root = find_project_root()
#     build_dir = root / "build"
#     if build_dir.exists():
#         shutil.rmtree(build_dir)
#         console.print("[green]✅ Полная очистка завершена[/]")
#     else:
#         console.print("[yellow]Нечего очищать[/]")
#
#
# @app.command()
# def reconfigure(verbose: bool = typer.Option(False, "--verbose", "-v")):
#     """Переконфигурировать CMake."""
#     root = find_project_root()
#     console.print(Panel.fit("[cyan]Переконфигурация CMake...[/]", border_style="cyan"))
#     run_cmake_configure(root, verbose)
#     console.print("[bold green]✅ Переконфигурация завершена![/]")
#
#
# @app.command()
# def monitor(
#     port: Optional[str] = typer.Option(None, "--port", "-p", help="COM-порт"),
#     baud: int = typer.Option(115200, "--baud", "-b", help="Скорость"),
# ):
#     """Монитор UART (как idf.py monitor)."""
#     if port is None:
#         console.print("[cyan]Автопоиск порта...[/]")
#         ports = [p.device for p in serial.tools.list_ports.comports()
#                  if any(x in p.description.lower() for x in ["ch340", "ch341", "wch", "usb"])]
#         if len(ports) == 1:
#             port = ports[0]
#             console.print(f"[green]Найден порт: {port}[/]")
#         else:
#             console.print("[red]Порт не найден или найдено несколько. Укажите --port[/]")
#             raise typer.Exit(1)
#
#     console.print(Panel.fit(
#         f"[bold green]Монитор запущен[/]\nПорт: [cyan]{port}[/] | Скорость: [cyan]{baud}[/]\n[dim]Ctrl+C — выход[/]",
#         border_style="green"
#     ))
#
#     try:
#         ser = serial.Serial(port, baud, timeout=0.1)
#         while True:
#             if ser.in_waiting:
#                 line = ser.readline().decode("utf-8", errors="replace").rstrip()
#                 if line:
#                     console.print(line)
#             time.sleep(0.01)
#     except KeyboardInterrupt:
#         console.print("\n[yellow]Монитор остановлен[/]")
#     except Exception as e:
#         console.print(f"[red]Ошибка: {e}[/]")
#     finally:
#         if 'ser' in locals():
#             ser.close()
#
#
# if __name__ == "__main__":
#     app()
