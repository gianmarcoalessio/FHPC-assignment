#!/bin/bash

# Sync the local directory to the remote directory
rsync -avz -e "ssh" --progress --exclude-from='exclude.txt' . orfeo:/u/dssc/galess00/final_assignment_FHPC/exercise1