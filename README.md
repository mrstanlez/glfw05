**This is an OpenGL project** with **pure C & glfw**, under **Linux & Windows 11**.\
Steps after downloading:
  1. extract all
  2. extract stb.zip (https://github.com/nothings/stb) Image & sound library for C
  3. Use this guide:

# Linux:\
(https://www.youtube.com/watch?v=gIMd63yb548&t=432s)

download & install: glfw 3.4, gcc 14, geany 2.0

**Arch/Manjaro:** sudo pacman -S geany-plugins geany gcc glfw\
**Debian/Mint/Ubuntu:** sudo apt-get install geany-plugins geany gcc glfw

**Geany:**\
Build/Set Build Commands/Compile(then click OK button): gcc "%f" -o "%e" -Wall -Wextra -lGL -lglfw -lm\
Build with F8 button, Run with F5 button

# Windows:\
(https://www.youtube.com/watch?v=9eySfe07PtI&t=5s)

download & install: glfw: glfw-3.4.bin.WIN64, x86_64-15.1.0-release-win32-seh-ucrt-rt_v12-rev0, geany-2.0_setup

Copy glfw3.dll from C:\glfw-3.4.bin.WIN64\lib-mingw-w64 to C:\Windows\System32\
Setup your windows Path: in extended setup system/Variable environments/System variables then select item: Path and

Click on button Change button Add: C:\mingw64\bin

**Geany:**\
Build/Set Build Commands/Compile(then click OK button): gcc "%f" -o "%e" -Wall -Wextra -I C:\glfw-3.4.bin.WIN64\include -L C:\glfw-3.4.bin.WIN64\lib-mingw-w64 -lglfw3dll -lopengl33\
Build with F8 button, Run with F5 button

Run in terminal:  ./main-test.exe\
PS: go to same directory as your compiled program is !

Thank you for support
