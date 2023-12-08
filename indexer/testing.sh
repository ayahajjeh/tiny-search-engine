 #
 #
 # testing.sh tests the indexer file
 #
 #
 # Aya Hajjeh, Oct 18, 2022
 # Lab5
 # CS50, Fall22
 #
 #


# PARAMETERS

# valid page directories
letters0=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-0
letters1=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-1
letters2=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-2
letters3=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-3
letters4=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-4
letters5=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letters-depth-5

toscrape0=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/toscrape-depth-0
toscrape1=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/toscrape-depth-1
toscrape2=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/toscrape-depth-2

wikipedia1=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/wikipedia-depth-1
wikipedia2=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/wikipedia-depth-2

# invalid page directories
notByCrawler=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output
pageDirectoryNotExistent=/thayerfs/courses/22fall/cosc050/workspace/tse/tse-output/letter

# test cases

# test case: letters depth 0
touch letters-0
echo "Testing on Letters depth 0; Saved in letters-0"
./indexer $letters0 letters-0 
if [ $? -ne 0 ]; then
    echo >&2 
    exit 1
fi
/thayerfs/courses/22fall/cosc050/workspace/tse/indexcmp $letters0 letters-0
