for i in {0..128}
do
    echo "#define _"$i"(x) x _"$[i-1]"(x)"
done
