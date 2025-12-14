#### chroot
changes the appernet root directory for a process and its children. It creates an isolated environment with its own `/`, `/bin`, `/lib`, etc.

It should be used to avoid polluting host environment with build dependencies or just use clean environment.

---

```bash
#!/bin/bash

root_dir="/home/otus"

sudo mkdir -p "$root_dir"

# 1. Create the chroot using debootstrap
sudo debootstrap \
	--include ncurses-term,mc,locales,nano,gawk,lsb-release,acl \
	--components=main,contrib,non-free \
	--arch=amd64 \
	bookworm \
	"$root_dir" \
	http://deb.debian.org/debian/
	
# 2. Copy DNS resolver config so chroot can resolve domain names
sudo cp /etc/resolv.conf "$root_dir/etc/resolv.conf"
 
# 3. Mount essential virtual filesystems into the chroot
sudo mount -t proc proc "$root_dir/proc"
sudo mount -t sysfs sys "$root_dir/sys"
sudo mount --rbind /dev "$root_dir/dev" 

# === inside env ===
# 4. Install dependencies
apt install -y build-essential libncurses-dev bison flex libssl-dev bc rsync libelf-dev debhelper-compat libdw-dev python3

# 5.Install kernel source code somewhere
git clone https://github.com/torvalds/linux.git
```

When you're done with chroot dir run
```bash
sudo umount "$root_dir/proc" "$root_dir/sys"
sudo umount -R "$root_dir/dev"  # -R for recursive unmount (handles --rbind)
```

To enter chroot env run
```bash
sudo chroot "$root_dir" /bin/bash
```

The next step is to create config file `.config`

```bash
make defconfig
```

Menu config
```bash
make menuconfig
```

### Options that MUST be DISABLED
DISABLED_OPTS=(
  CONFIG_SECURITY_SELINUX
  CONFIG_SECURITY_SMACK
  CONFIG_SECURITY_TOMOYO
  CONFIG_SECURITY_APPARMOR
  CONFIG_SECURITY_YAMA
  CONFIG_RANDOMIZE_BASE
  CONFIG_CPU_MITIGATIONS
  CONFIG_MITIGATION_SPECTRE_BHI
  CONFIG_MITIGATION_RFDS
  CONFIG_PAGE_TABLE_ISOLATION
  CONFIG_BPF
  CONFIG_BPF_SYSCALL
  CONFIG_BPF_JIT
  CONFIG_BPF_EVENTS
  CONFIG_BPFILTER
)
### Options that MUST be ENABLED (=y)
ENABLED_OPTS=(
  CONFIG_DEBUG_FS
  CONFIG_FTRACE
  CONFIG_FUNCTION_TRACER
  CONFIG_DYNAMIC_FTRACE
  CONFIG_FUNCTION_GRAPH_TRACER
  CONFIG_STACK_TRACER
  CONFIG_KUNIT
  CONFIG_KUNIT_TEST
  CONFIG_KASAN
  CONFIG_STACKTRACE
  CONFIG_KASAN_GENERIC
  CONFIG_KASAN_INLINE
  CONFIG_KASAN_EXTRA_INFO
  CONFIG_KGDB
  CONFIG_KGDB_SERIAL_CONSOLE
  CONFIG_DEBUG_INFO
  CONFIG_SERIAL_CONSOLE
  CONFIG_CONSOLE_POLL
  CONFIG_KPROBES
  CONFIG_KPROBE_EVENT
)

Собираем в пакет =)
```bash
make bindeb-pkg
```

```bash
linux-image-<version>-<arch>.deb      # Kernel + modules
linux-headers-<version>-<arch>.deb    # Headers for modules
linux-libc-dev_<version>.deb          # Optional: userspace headers
```

---

## Installing on VM

```bash
sudo dpkg -i linux-*.deb
sudo update-grub
sudo reboot
uname -r # to check version, should be 6.19.0-rc1
dmesg | grep -i error   # to check Kernel errors
```