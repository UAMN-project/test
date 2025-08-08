/* 这是一个测试程序，用于展示ToyC编译器的功能 */

// 计算斐波那契数列第n项
int fib(int n) {
    if (n <= 1) {
        return n;
    }
    return fib(n - 1) + fib(n - 2);
}

// 计算最大公约数
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// 判断是否是素数
int isPrime(int n) {
    if (n <= 1) {
        return 0;
    }
    
    int i = 2;
    while (i * i <= n) {
        if (n % i == 0) {
            return 0;
        }
        i = i + 1;
    }
    return 1;
}

// 主函数，测试各种功能
int main() {
    // 变量声明和初始化
    int x = 10;
    int y = 15;
    
    // 算术运算和函数调用
    int result = gcd(x, y);
    
    // if-else语句和函数调用
    if (isPrime(result)) {
        result = fib(result);
    } else {
        result = result * 2;
    }
    
    // while循环和break
    int i = 0;
    while (i < 10) {
        if (i == 5) {
            break;
        }
        i = i + 1;
    }
    
    // 返回最终结果
    return result;
}