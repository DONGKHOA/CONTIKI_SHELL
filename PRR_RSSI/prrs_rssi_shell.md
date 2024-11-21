# PRR & RSSI TABLE SHELL

## Introduction

Hiển thị 1 bảng gồm các node láng giềng được sắp xếp giảm dần theo chỉ số PRR hoặc RSSI.

## How To Use

Tạo và đưa 2 file dưới đây vào thư mục sau: _~/contiki-2.7/apps/shell_

1 - shell-try-example.c
2 - shell-try-example.h

Chỉnh sửa Makefile.shell ở _~/contiki-2.7/apps/shell_ để thêm _shell-try-example.c_ trong shell_src

```
shell_src = ... shell-try-example.c ...
```

Tạo và thêm file _sky-shell.c_ ở đường dẫn _ ~/contiki-2.7/examples_

Thêm Makefile cho project

```
CONTIKI_PROJECT = mySky-shell
all: $(CONTIKI_PROJECT)

APPS = serial-shell powertrace collect-view
CONTIKI = ../..

include $(CONTIKI)/Makefile.include
-include /home/user/nes/testbed-scripts/Makefile.include
```

## Command

- Sử dụng lệnh:

```
nntable

```

Chạy mô phỏng với cooja (3 nodes),
