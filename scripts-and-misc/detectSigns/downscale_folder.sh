for item in ./*; do
	convert $item -resize 1600x1200 $item
done

