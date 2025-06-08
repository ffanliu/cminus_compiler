### 一个不完整的 C- 语言编译器实现，包括词法分析器、语法分析器和抽象语法树（AST）生成。该项目是编译原理课程的课程设计，实现了 C- 语言的词法和语法分析功能

#### 项目功能
词法分析：将源代码转换为 Token 序列
语法分析：使用递归下降法构建抽象语法树
错误处理：显示语法错误位置和原因
AST 可视化：支持文本格式的语法树输出

### 快速开始
#### 构建项目
# 克隆仓库
git clone https://github.com/ffanliu/cminus_compiler.git
cd cminus_compiler

# 创建构建目录
mkdir build
cd build

# 使用 CMake 构建项目
cmake ..
make

#### 运行编译器

解析测试文件
./cminus_compiler ../test.cm

输出 AST
./cminus_compiler ../test.cm --ast

示例代码

test.cm 文件内容：
int main() {
    int a = 10;
    if (a > 5) {
        return a;
return 0;

输出结果

Tokens:
[INT] int
[ID] main
[LPAREN] (
[RPAREN] )
[LBRACE] {
[INT] int
[ID] a
[ASSIGN] =
[NUM] 10
[SEMI] ;
[IF] if
[LPAREN] (
[ID] a
[GT] >
[NUM] 5
[RPAREN] )
[LBRACE] {
[RETURN] return
[ID] a
[SEMI] ;
[RBRACE] }
[RETURN] return
[NUM] 0
[SEMI] ;
[RBRACE] }

AST:
Program
  Function: main (return: int)
    Body:
      Declaration: a (int) = 10
      IfStatement
        Condition: a > 5
        Then:
          Return: a
      Return: 0

项目结构

cminus_compiler/
├── CMakeLists.txt      # 构建配置文件
├── include/            # 头文件
├── lexer.h         # 词法分析器

└── parser.h        # 语法分析器

├── src/                # 源代码
├── lexer.cpp       # 词法分析实现

├── parser.cpp      # 语法分析实现

└── main.cpp        # 主程序

└── test.cm             # 测试文件

如何测试

运行编译后的程序

./build/cminus_compiler test.cm

尝试修改 test.cm 文件测试不同代码
