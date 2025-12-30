# NoAxiom-OS 环境搭建与实验指南

## 一、 源码获取与初始化
首先克隆主仓库并同步所有依赖的子模块。

1. **克隆仓库**
   ```bash
   git clone https://github.com/NoAxiom/NoAxiom-OS.git
   cd NoAxiom-OS
   ```

2. **初始化子模块**
   ```bash
   # 递归初始化并更新子模块（如内核、用户态、测试用例等）
   git submodule update --init --recursive
   ```
---

## 二、 开发环境配置

### 1. 安装 Rust 编译环境
为了加快下载速度，建议使用国内镜像源。安装过程中遇到选项直接按 **回车** 使用默认配置即可。

```bash
# 设置 Rust 镜像环境变量
export RUSTUP_DIST_SERVER="https://rsproxy.cn"
export RUSTUP_UPDATE_ROOT="https://rsproxy.cn/rustup"

# 下载并运行 Rust 安装脚本
curl --proto '=https' --tlsv1.2 -sSf https://rsproxy.cn/rustup-init.sh | sh

# 刷新当前 Shell 环境变量
source $HOME/.cargo/env
```

### 2. 安装交叉编译组件与 Git LFS
```bash
# 安装 RISC-V 交叉编译目标
make add-target

# 安装 Git 大文件存储支持（Git LFS）
apt-get update && apt-get install git-lfs -y
git lfs install

# 同步 LFS 资源及子模块更新
make git-update
```

---

## 三、 内核编译与依赖处理

在首次编译时，通常会因为缺少第三方库依赖包而报错，需要手动处理 `vendor` 离线包。

### 1. 处理用户态与内核依赖
```bash
# 1. 进入用户态目录处理依赖
cd NoAxiom-OS-User
make vendor
cd ..

# 2. 进入内核目录处理依赖
cd NoAxiom/kernel
# 根据 Cargo.toml 下载依赖包并存放到 vendor 目录
cargo vendor ../vendor
cd ../..
```

### 2. 安装二进制工具集
编译内核镜像需要 `cargo-binutils`。如果安装时提示版本冲突，请使用 `--locked` 模式。

```bash
# 使用锁定版本模式安装工具集，规避 nightly 版本兼容性问题
cargo install cargo-binutils --locked
```

### 3. 执行编译
```bash
# 编译生成 RISC-V 架构内核镜像
make build
```
编译成功后，生成的镜像文件位于：`NoAxiom/target/riscv64gc-unknown-none-elf/release/kernel.bin`

---

## 四、 使用 QEMU 运行与调试

### 1. 安装模拟器
```bash
apt-get update
apt-get install -y qemu-system-misc qemu-system-riscv64
```

### 2. 修复运行脚本环境
由于脚本可能在 Windows 环境下编辑过，需处理权限与回车符（CRLF）问题。

```bash
# 安装格式转换工具
apt-get install dos2unix -y

# 赋予解压脚本执行权限
chmod +x /root/NoAxiom-OS/NoAxiom-OS-Test/utils/extract.sh

# 消除 Windows 换行符影响
dos2unix /root/NoAxiom-OS/NoAxiom-OS-Test/utils/extract.sh

# 手动创建运行日志目录
mkdir -p log
```

### 3. 配置 OpenSBI 固件
QEMU 启动 RISC-V 内核需要 Bootloader（OpenSBI）。

```bash
# 下载 OpenSBI 固件（使用加速代理）
wget https://ghproxy.net/https://github.com/riscv-software-src/opensbi/releases/download/v1.2/opensbi-1.2-rv-bin.tar.xz

# 解压并拷贝至项目根目录，重命名为 Makefile 识别的名称
tar -xf opensbi-1.2-rv-bin.tar.xz
cp opensbi-1.2-rv-bin/share/opensbi/lp64/generic/firmware/fw_jump.bin ./opensbi-riscv64-virt-fw_jump.bin
```

### 4. 启动内核
```bash
make run
```

---

## 五、 多架构支持（LoongArch 编译）

若需尝试龙芯架构（LoongArch）的编译，需安装对应的交叉编译工具链。

### 1. 安装龙芯交叉编译器
```bash
# 下载工具链
wget https://ghproxy.net/https://github.com/loongson/build-tools/releases/download/2022.09.06/loongarch64-clfs-6.3-cross-tools-gcc-glibc.tar.xz

# 解压工具链
tar -xf loongarch64-clfs-6.3-cross-tools-gcc-glibc.tar.xz
```
解压后，会得到一个 cross-tools 文件夹。

### 2. 适配工具链名称
Makefile 中硬编码的工具名可能与下载的文件名略有出入，需建立软链接并配置环境变量。

```bash
# 进入工具链 bin 目录
cd /root/NoAxiom-OS/cross-tools/bin

# 批量建立局部链接，将 unknown-linux-gnu 映射为 linux-gnu
for f in loongarch64-unknown-linux-gnu-*; do 
    ln -sf "$f" "${f/unknown-linux-gnu/linux-gnu}"; 
done

# 回到项目根目录并设置环境变量
cd /root/NoAxiom-OS
export PATH=$(pwd)/cross-tools/bin:$PATH

# 验证安装
loongarch64-linux-gnu-gcc --version
```

### 3. 全架构编译
```bash
# 编译所有支持的架构镜像
make all
```
---
**提示：** 在实验过程中，若遇到权限不足问题，请确保使用 `sudo` 或在 `root` 用户下操作；若下载过慢，请检查 `ghproxy.net` 等代理前缀是否可用。