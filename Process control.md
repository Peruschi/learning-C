# 流程控制
### 短程控制 `goto`
相信大部分人剛開始接觸C語言，都會被告知別用`goto`這個標籤，所有的問題皆可以靠基本的`if` `else` `for` `while`這類簡單的流程來實現。  
但在適當的時機使用它，反而能大大的增加程式碼的可讀性、減少開發時間甚至提升效率，像是Linux Kernel裡面其實也用了不少。

`goto`使用起來很方便，可以直接將程式跳轉到標籤定義的地方繼續執行。(不過也是因為這樣，所以會導致debug或是維持程式的難度上升)  
`goto`又被叫做短程控制，它跟區域變數有點類似，沒辦法從一個函數直接跳轉到另一個函數。
#### 一般用法
在同一個函式內`goto`會跳轉到，標籤定義的地方繼續執行。
```c
#include<stdio.h>

int main(void) {
    int a = 100;
    int b = 0;
    
    if (b == 0){
        goto not_divided;
    }
    printf("a // b = %d\n", a / b);
    return 0;
    
not_divided:
    printf("a cannot be divided by b!\n");
    return 0;
}
```
#### 錯誤用法
`goto`和標籤定義在不同函式，會產生編譯錯誤。
```c
#include<stdio.h>

int divide(int a, int b){
    if (b == 0){
        goto not_divided;
    }
    printf("a // b = %d\n", a / b);
}

int main(void) {
    int a = 100;
    int b = 0;
    divide(a, b);
    return 0;
    
not_divided:
    printf("a cannot be divided by b!\n");
    return 0;
}
```
___
### 長程控制 `setjmp`
跟上面所講的，最直接的差別就是使用setjmp可以實現函式之間的跳轉。  

```c
#include<stdio.h>
#include<setjmp.h>

static jmp_buf env1, env2;

void first(){
    printf("first\n");
    longjmp(env2, 1);
    longjmp(env1, 1);
}

int main(void){
    if (!setjmp(env1)){
        if (setjmp(env2)){
            printf("return env2\n");
            return 0;
        }
        first();
    }
    printf("return env1\n");
}
```
#### 輸出結果
```
first
return env2
```
在上面的程式碼，有三個特別的東西，也是包含在`<setjmp.h>`標頭檔中。  
- `jmp_buf`: 是一個大小為10的array，存著控制跳轉所需的各類變數
- `setjmp`: 是一個macro，`int setjmp(jmp_buf environment)`是它的型態
- `longjmp`: 是一個函式，`void longjmp(jmp_buf environment, int value)`是它的型態

使用它前，我們需要先宣告一個`jmp_buf`類型的變數，它保存了當前執行的環境，紀錄需要跳轉後的樣子。  
它藉由當作參數傳入`setjmp`，macro`setjmp`不僅僅只會回傳一次，但最初值都是回傳0，  
它會根據之後調用的函式`longjmp`跳回`setjmp`的位置，並且由`longjmp`的參數`value`去更改原先`setjmp`的回傳值。  
我可以借由她回傳值的改變來控制流程的變化。


有興趣在深入了解的可以去翻翻`<setjmp.h>`源碼，並不只包含C語言，還內含有一些組合語言的內容，適合了解組語的人。
