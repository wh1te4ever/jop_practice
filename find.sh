#grep -E -o '\bmov[[:space:]]+x[0-9]+,[[:space:]]*x[0-9]+ ; br x[^;] ;' mov_br.txt > mov_br2.txt

grep -E -o '\bmov[[:space:]]+x[0-9]+,[[:space:]]*x[0-9]+ ; br [^;]*' mov_br.txt | sort | uniq > mov_br2.txt