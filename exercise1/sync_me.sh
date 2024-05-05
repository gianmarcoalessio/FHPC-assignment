#!/bin/bash
#Sync the remote directory thin to the local directory
rsync -avz -e "ssh" --progress orfeo:/u/dssc/galess00/final_assignment_FHPC/exercise1/thin .
