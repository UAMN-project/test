#!/usr/bin/env python3
import subprocess
import os
import sys

def run_test(test_file, expected_output):
    """运行单个测试用例"""
    try:
        # 编译测试文件
        cmd = [sys.executable, "compiler.py", test_file]
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=10)
        
        if result.returncode != 0:
            print(f"❌ 编译失败: {test_file}")
            print(f"错误: {result.stderr}")
            return False
            
        # 比较输出
        actual_output = result.stdout.strip()
        expected_output = expected_output.strip()
        
        if actual_output == expected_output:
            print(f"✅ 通过: {test_file}")
            return True
        else:
            print(f"❌ 输出不匹配: {test_file}")
            print(f"期望: {repr(expected_output)}")
            print(f"实际: {repr(actual_output)}")
            return False
            
    except subprocess.TimeoutExpired:
        print(f"❌ 超时: {test_file}")
        return False
    except Exception as e:
        print(f"❌ 异常: {test_file} - {e}")
        return False

def main():
    """运行所有测试用例"""
    test_cases = [
        ("test/basic.c", "main:\n\taddi sp, sp, -128\n\taddi sp, sp, 128\n\tret\n"),
        ("test/arithmetic.c", ""),  # 算术运算测试
        ("test/variables.c", ""),   # 变量测试
        ("test/loops.c", ""),       # 循环测试
        ("test/conditions.c", ""),  # 条件测试
    ]
    
    passed = 0
    total = len(test_cases)
    
    for test_file, expected in test_cases:
        if run_test(test_file, expected):
            passed += 1
    
    print(f"\n测试结果: {passed}/{total} 通过")
    return 0 if passed == total else 1

if __name__ == "__main__":
    sys.exit(main())