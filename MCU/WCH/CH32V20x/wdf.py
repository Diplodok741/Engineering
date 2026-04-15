#!/usr/bin/env python3
"""
wdf.py — аналог idf.py для CH32V20x с официальным MounRiver toolchain
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
    help="🔧 wdf.py — утилита сборки для CH32V20x",
    add_completion=True,
    no_args_is_help=True,
)

TOOLCHAIN_BIN = "/home/blank/Engineering/MCU/WCH/toolchain/riscv32/bin"


def find_project_root() -> Path:
    current = Path.cwd().resolve()
    while current != current.parent:
        if (current / "CMakeLists.txt").exists() and (current / "wdf.py").exists():
            return current
        current = current.parent
    console.print(Panel("[bold red]Не найден корень проекта (CMakeLists.txt + wdf.py)![/]",
                       border_style="red", title="Ошибка"))
    raise typer.Exit(1)


def get_current_project_dir() -> Path:
    cwd = Path.cwd().resolve()
    root = find_project_root()
    if cwd.is_relative_to(root / "Project"):
        return cwd

    default = root / "Project" / "blink"
    if default.exists():
        console.print(Panel(f"[yellow]Запущено из корня. Используется проект по умолчанию: blink[/]",
                           border_style="yellow", title="Предупреждение"))
        return default

    console.print(Panel("[bold red]Папка Project/blink не найдена![/]", border_style="red"))
    raise typer.Exit(1)


def ensure_build_dir(project_dir: Path) -> Path:
    build_dir = project_dir / "build"
    build_dir.mkdir(exist_ok=True)
    return build_dir


def check_toolchain():
    """Проверка официального тулчейна"""
    gcc = Path(TOOLCHAIN_BIN) / "riscv32-wch-elf-gcc"
    if not gcc.exists():
        console.print(Panel("[bold red]Официальный тулчейн не найден![/]",
                           border_style="red", title="Ошибка"))
        console.print(f"   Ожидаемый путь: {gcc}")
        raise typer.Exit(1)
    
    console.print(Panel(f"[green]✓ Toolchain: {gcc.name} 15.2.0[/]",
                       border_style="green", title="Тулчейн OK"))


def run_cmake_configure(project_dir: Path, root: Path, verbose: bool = False):
    build_dir = ensure_build_dir(project_dir)
    console.print(Panel.fit(
        f"[cyan]Конфигурация CMake для проекта: [bold]{project_dir.name}[/][/]",
        border_style="cyan"
    ))

    cmd = [
        "cmake",
        "-B", str(build_dir),
        "-S", str(root),
        "-G", "Ninja",
        f"-DPROJECT_DIR={project_dir}",
        "-DCMAKE_BUILD_TYPE=Release",
        f"-DCMAKE_C_COMPILER={TOOLCHAIN_BIN}/riscv32-wch-elf-gcc",
        f"-DCMAKE_ASM_COMPILER={TOOLCHAIN_BIN}/riscv32-wch-elf-gcc",
        f"-DCMAKE_OBJCOPY={TOOLCHAIN_BIN}/riscv32-wch-elf-objcopy",
        f"-DCMAKE_OBJDUMP={TOOLCHAIN_BIN}/riscv32-wch-elf-objdump",
        f"-DCMAKE_SIZE={TOOLCHAIN_BIN}/riscv32-wch-elf-size",
    ]

    if verbose:
        cmd.append("-DCMAKE_VERBOSE_MAKEFILE=ON")

    try:
        subprocess.run(cmd, cwd=root, check=True)
    except subprocess.CalledProcessError:
        console.print(Panel("[bold red]Ошибка конфигурации CMake[/]",
                           border_style="red", title="Ошибка"))
        raise typer.Exit(1) from None


def build_firmware(root: Path, verbose: bool = False):
    check_toolchain()
    project_dir = get_current_project_dir()
    run_cmake_configure(project_dir, root, verbose)

    build_dir = project_dir / "build"
    console.print(Panel.fit("[cyan]Выполняется сборка...[/]", border_style="cyan"))

    cmd = ["cmake", "--build", str(build_dir)]
    if verbose:
        cmd.append("--verbose")

    try:
        subprocess.run(cmd, cwd=root, check=True)
        console.print(Panel("[bold green]Сборка завершена успешно![/]",
                           border_style="green", title="Успех"))
        show_size(build_dir, root)
    except subprocess.CalledProcessError:
        console.print(Panel("[bold red]Ошибка сборки![/]",
                           border_style="red", title="Ошибка"))
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
        console.print(Panel("[bold green]Прошивка завершена успешно![/]",
                           border_style="green", title="Успех"))
    except subprocess.CalledProcessError:
        console.print(Panel("[bold red]Ошибка прошивки![/]",
                           border_style="red", title="Ошибка"))


@app.command()
def clean():
    """Очистить build в текущей папке проекта"""
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
    if build_dir.exists():
        shutil.rmtree(build_dir)
        console.print(Panel(f"[green]build очищен в {project_dir.name}[/]",
                           border_style="green", title="Успех"))
    else:
        console.print(Panel("[yellow]build уже отсутствует[/]", border_style="yellow"))


@app.command()
def size():
    """Показать размер прошивки"""
    root = find_project_root()
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
    if not build_dir.exists():
        console.print(Panel("[red]Сначала выполните сборку (build)[/]",
                           border_style="red", title="Ошибка"))
        raise typer.Exit(1)
    show_size(build_dir, root)


@app.command()
def erase():
    """Стереть чип"""
    root = find_project_root()
    project_dir = get_current_project_dir()
    build_dir = project_dir / "build"
   
    console.print(Panel("[red]Стираем чип...[/]", border_style="red", title="Внимание"))
    try:
        subprocess.run(
            ["cmake", "--build", str(build_dir), "--target", "erase"],
            cwd=root,
            check=True,
        )
        console.print(Panel("[bold green]Чип стёрт![/]", border_style="green", title="Успех"))
    except subprocess.CalledProcessError:
        console.print(Panel("[bold red]Ошибка при стирании чипа[/]", border_style="red", title="Ошибка"))


@app.command()
def monitor(
    port: Optional[str] = typer.Option(None, "--port", "-p"),
    baud: int = typer.Option(115200, "--baud", "-b"),
):
    """Монитор UART"""
    if port is None:
        ports = [p.device for p in serial.tools.list_ports.comports()
                 if any(x in p.description.lower() for x in ["ch340", "ch341", "wdf", "usb"])]
        port = ports[0] if ports else None
        if not port:
            console.print(Panel("[red]Порт не найден. Укажите --port[/]",
                               border_style="red", title="Ошибка"))
            raise typer.Exit(1)

    console.print(Panel.fit(
        f"[green]Монитор запущен на {port} @ {baud} бод[/]\n[dim]Ctrl+C для выхода[/]",
        border_style="green"
    ))

    ser = None
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
        if ser:
            ser.close()


if __name__ == "__main__":
    app()
