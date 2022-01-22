# Variadic
### 可變參數函式
我們剛接觸程式語言，不管是哪一個都是先來輸出個`"Hello World!"`。  
在C中函式`printf`就是用來輸出的，`int printf(const char *format, ...)`是它的樣式，在這個函式的參數中有個比較特別的參數`...`，它就是C語言用來表達**可變參數**，意即參數的數量不定。  

```c
#include<stdio.h>
#include<stdarg.h>

void printList(int head, ...){
    va_list args;
    va_start(args, head);
    
    for (int i = head; i != -1; i = va_arg(args, int)){
        printf("%d\n", i);
    }
    
    va_end(args);
}

int main(void){
    printList(4, 5, 6, 7, 8, 9, -1);
}
```
#### 輸出結果
```
4
5
6
7
8
9
```
上面的程式碼有幾個我們需要關注的點。  
- `va_list`: 型態是指標，可能是`void*`或`char*`，依據不同編譯器，可能會有不同結果。
- `va_start`: 是一個marco，樣式`void va_start(va_list arg_ptr, last_arg)`。
- `va_arg`: 是一個marco，樣式`type va_arg(va_list arg_ptr, type)`。
- `va_end`: 是一個marco，樣式`void va_end(va_list arg_ptr)`。

以上這些都是包含在C的<stdarg.h>中。  
在函式的參數以`...`代替一般參數，之後再調用函式時就可以填入任意數量的參數。  

`arg_ptr`統一需要放宣告在前面`va_list`的變數。  
`va_start`用來抓取最後一位固定的參數。  
`va_arg`則是用來抓取那些任意數量的參數，`type`指的是傳入參數的資料型態。  
`va_end`用來表示終止檢索那些傳入任意參數。  

這裡有幾個重點
1. **宣告的固定參數在調用時不能少**
2. 之後的任意數量的**參數型態都要一樣**
3. macro`va_arg`抓參數時，**沒法辨認有多少個參數**，**不知何時終止**，需要自己處理。
4. **參數的型態有許多限制**，大多數都不能用，或者會被強制轉成`int` `unsigned int` `double`，下面這些不能使用。
- char、signed char、unsigned char、short、unsigned short、signed short、short int、signed short int、unsigned short int、float
5. C99標準添加了`va_copy`

這裡提一個我覺得比較重要的一點，我們可能會有個疑惑，為啥C設計時要我們設計師自己處理參數的數量。  
問題點就是`va_arg`這個marco取得參數的設計，由於函式在堆疊區展開時參數的地址是連續的，`va_arg`抓取參數時是由指標移動偏移量取得的，這樣就沒法得知我們傳入了多少參數。  

我們較常處理這問題的方法:
1. 傳入一個數字，告知函式有多少個參數需要處理
2. 傳入一個終止符，在檢索到終止符時自動停止

___

### 可變參數marco

我們在定義marco也可以像函式使用`...`，表達可變參數。  
```c
#include<stdio.h>


// __VA_ARGS__是原本C用來表達可變參數的替代詞
#define feedback1(...) ( \
            printf(__VA_ARGS__) \
        )

// 如果我們不想用C定義的__VA_ARGS__
// 我們可以自己定義一個名稱，格式如下
#define feedback2(args...) ( \
            printf(args) \
        )

// 下面的相比上面多個##
// 如果我們沒有傳入參數給args
// 那麼按照define的功能，文字替代
// 文字替代後會產生printf(format, )
// 這樣因為有','所以編譯會有問題
// ##有個功能就是在沒有參數傳入的情況下
// 消掉多餘的','，使得結果變成printf(format)
#define my_print(format, args...) ( \
            printf(format, ##args) \
        )

int main(void) {
    feedback1("feedback1: I'm hungry.\n");
    feedback2("feedback2: I'm hungry.\n");
    my_print("my_print: I'm hungry.\n");
    my_print("my_print: num1 = %d, num2 = %d\n", 45, 54);
    return 0;
}
```
#### 輸出結果
```
feedback1: I'm hungry.
feedback2: I'm hungry.
my_print: I'm hungry.
my_print: num1 = 45, num2 = 54
```
marco的可變參數，沒辦法單獨讀取，意即我們沒辦法像函式`va_arg`一次次抓取變數，所以相對於函式有時候並沒法簡單值觀的實現一些方法。
