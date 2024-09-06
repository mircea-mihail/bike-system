# convert $item -resize 352x288 $item
give_way_path="./my-dataset/give-way"
original_dataset="the_dataset"
downscale_rez="352x288"
downscale_name="downscaled_$downscale_rez"

for dataset in "$give_way_path"/*; do
	if [ ! -e "$dataset/$downscale_name" ]; then
		mkdir "$dataset/$downscale_name"
	fi
	 
	for picture in "$dataset/$original_dataset"/*; do
		picture_name=$(basename $picture)
		convert $picture -resize $downscale_rez "$dataset/$downscale_name/$picture_name"
	done
done

