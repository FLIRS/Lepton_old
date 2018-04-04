echo 17 > /sys/class/gpio/unexport
echo 17 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio17/direction
echo rising > /sys/class/gpio/gpio17/edge
cat /sys/class/gpio/gpio17/value
