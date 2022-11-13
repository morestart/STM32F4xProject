# Tools

## us延时

```c
int main(void) {
	// 初始化延时函数 168是时钟分频
	delay_init(168);

	while (1) {

	}
}
```

## print串口打印

```c
// 先注册串口
RetargetInit(&huart6);
print("%d", 123);
```