# BLINK LED SHELL

## Introduction
Thực hiện tạo 1 shell blink 3 led cùng lúc theo số lần mong muốn “blink [nums]” và tạo 1 shell “led” để blink từng led mỗi led 3s.

## Command

- Sử dụng Command sau để blink led theo 2 mẫu shell mô tả ở Introduction:

```
blink [nums] | led

``` 
Chỉnh sửa file: shell-blink.c trong đường dẫn contiki/apps/shell
Chạy mô phỏng trên cooja: thực hiện tạo 1 node và nạp code sky-shell.c trong đường dẫn contiki/examples/sky-shell, mở serial và show led của node. Ta nhập lệnh ví dụ muốn blink cả 3 led 3 lần là “blink 3” hoặc nếu muốn blink từng led để kiểm tra các led ta thực hiện nhập shell “led”

- Kết quả thực hiện: chạy file blink_led_shell.csc để thực hiện mô phỏng 