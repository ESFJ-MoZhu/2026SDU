CC = gcc
CFLAGS = -O3 -Wall -Wextra -std=c99 -march=native -funroll-loops
LDFLAGS = -lm
SRCDIR = .
OBJDIR = obj
BINDIR = bin
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/sm3_project
.PHONY: all clean test benchmark
all: $(TARGET)
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR):
	mkdir -p $(OBJDIR)
$(BINDIR):
	mkdir -p $(BINDIR)
clean:
	rm -rf $(OBJDIR) $(BINDIR)
test: $(TARGET)
	./$(TARGET)
benchmark: $(TARGET)
	@echo "运行性能测试..."
	@time ./$(TARGET)
debug: CFLAGS = -O0 -g -Wall -Wextra -std=c99 -DDEBUG
debug: $(TARGET)
release: CFLAGS = -O3 -Wall -Wextra -std=c99 -march=native -funroll-loops -DNDEBUG
release: $(TARGET)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
help:
	@echo "可用目标:"
	@echo "  all      - 编译项目 (默认)"
	@echo "  clean    - 清理编译文件"
	@echo "  test     - 运行测试"
	@echo "  benchmark - 运行性能测试"
	@echo "  debug    - 编译调试版本"
	@echo "  release  - 编译发布版本"
	@echo "  install  - 安装到系统"
	@echo "  help     - 显示帮助信息"