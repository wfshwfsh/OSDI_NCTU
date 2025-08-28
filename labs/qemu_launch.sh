# PL011-Uart
#qemu-system-aarch64 -M raspi3b -m 1G -kernel kernel.img -serial stdio -display none

# mini-Uart
qemu-system-aarch64 -M raspi3b -m 1G -kernel kernel.img -serial null -serial stdio -display none

# GDB
#qemu-system-aarch64 -M raspi3 -m 1G -kernel kernel.img -serial stdio -display none -S -s

