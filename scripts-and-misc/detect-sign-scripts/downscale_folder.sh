# convert $item -resize 352x288 $item
give_way_path="./my-dataset/give-way/esp-cam"
original_dataset="new-set"
downscale_rez="320x240"
downscale_name="downscaled_$downscale_rez"

for dataset in "$give_way_path"/*; do
	if [ ! -e "$dataset/$downscale_name" ]; then
		echo "downscaling pictures from $dataset..."
		mkdir "$give_way_path/$downscale_name"

		current_downscaled_dir="$downscale_name-$original_dataset"
		for picture in "$dataset"/*; do
			picture_name=$(basename $picture)
			convert $picture -resize $downscale_rez "$give_way_path/$downscale_name/$picture_name"
		done
	fi
done

