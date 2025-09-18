PARAM_DISP=$1

option_disp=""

if [[ "$PARAM_DISP" != "display" ]]; then
    option_disp="-display none"
fi

# PL011-Uart
#qemu-system-aarch64 -M raspi3b -m 1G -kernel kernel.img -serial stdio -display none

# mini-Uart
qemu-system-aarch64 -M raspi3b -m 1G -kernel kernel.img -serial null -serial stdio ${option_disp}

# GDB
#qemu-system-aarch64 -M raspi3b -m 1G -kernel kernel.img -serial null -serial stdio ${option_disp} -S -s

