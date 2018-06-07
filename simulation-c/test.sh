for i in $(seq 1 $1); do
    ./simulation > /dev/null
    if [ $? -ne 0 ]; then
        echo ERROR
    fi
done
