//Steven Montalbano (smm285)

# include <stdio.h>
# include <string.h>
# include <stdint.h>
# include <ctype.h>
# include <stdlib.h>

typedef struct wavHeader {
  char riff_id[4];
  uint32_t file_size;
  char wave_id[4];
  char fmt_id[4];
  uint32_t fmt_size;
  uint16_t data_format;
  uint16_t  number_of_channels;
  uint32_t  samples_per_second;
  uint32_t  bytes_per_second;
  uint16_t block_alignments;
  uint16_t bits_per_sample;
  char data_id[4];
  uint32_t data_size;
} wavHeader;

int streq_nocase(const char* a, const char* b) {  //compare string, case insensitive
	for(; *a && *b; a++, b++)
    if(tolower(*a) != tolower(*b))
      return 0;

	return *a == 0 && *b == 0;
}

void showMenu() {
  printf("\nWelcome to WavEdit!  A program for manipulating .wav files.\n");
  printf("Usage:\n");
  printf("  ./wavedit [FILE.wav]             : Display the files properties\n");
  printf("  ./wavedit [FILE.wav] -reverse    : Reverse the audio \n");
  printf("  ./wavedit [FILE.wav] -rate [INT] : Change the sample rate\n");
}

int confirmFileType(wavHeader file) {

  if(strncmp(file.riff_id, "RIFF", 4) != 0)
    return 0;

  if(strncmp(file.wave_id, "WAVE", 4) != 0)
    return 0;

  if(strncmp(file.fmt_id, "fmt ", 4) != 0)
    return 0;

  if(strncmp(file.data_id, "data", 4) != 0)
    return 0;

  if(file.fmt_size != 16)
    return 0;

  if(file.data_format != 1)
    return 0;

  if (!(file.number_of_channels == 1 || file.number_of_channels == 2))
    return 0;

  if (!(file.samples_per_second > 0 && file.samples_per_second <= 192000))
    return 0;

  if (!(file.bits_per_sample == 8 || file.bits_per_sample == 16))
    return 0;

  if(file.bytes_per_second != file.samples_per_second * (file.bits_per_sample)/8 * file.number_of_channels)
    return 0;

  if(file.block_alignments != (file.bits_per_sample)/8 * file.number_of_channels)
    return 0;

    return 1;
}

void showFileInfo(wavHeader file, char* filename) {

  if (confirmFileType(file) == 0) {
      printf("File is not a valid .wav file\n");
      exit(1);
    }

  printf("%s properties:\n", filename);

  printf("This is a %i-bit %iHz %s sound file.\n", file.bits_per_sample,
    file.samples_per_second, (file.number_of_channels - 1) ? "stereo" : "mono" );

  printf("It is %i samples (%.3f seconds) long.\n", (file.data_size / file.block_alignments),
    (float)(file.data_size / file.block_alignments) / file.samples_per_second);

}

void changeSampleRate(char* filename,  char* rate) {

  int newRate = atoi(rate); //cast string to int

  if ((newRate < 0) || (newRate > 192000)) {
    printf("Sample rate %i not in range of valid rates.\n", newRate);
    exit(1);
  }

  wavHeader soundFile;

  FILE* file = fopen(filename, "rb+");

  fread(&soundFile, sizeof(wavHeader), 1, file);//read the file header into a struct

  if (confirmFileType(soundFile) == 0) {
    printf("File is not a valid .wav file\n");
    exit(1);
  }

  soundFile.samples_per_second = newRate; //change sample rate
  soundFile.bytes_per_second = (soundFile.samples_per_second *  //change bit rate
    (soundFile.bits_per_sample)/8 * soundFile.number_of_channels);

  fseek(file, 0, SEEK_SET);   //return to beginning of file
  fwrite(&soundFile, sizeof(wavHeader), 1, file);   //write changes to file
  fclose(file);
}

void reverse(char* filename) {

  wavHeader soundFile;

  FILE* file = fopen(filename, "rb+");

  fread(&soundFile, sizeof(wavHeader), 1, file);

  if (confirmFileType(soundFile) == 0) {
    printf("File is not a valid .wav file\n");
    exit(1);
  }

  int sampleLength = soundFile.data_size / soundFile.block_alignments;

  if(soundFile.bits_per_sample == 8 && soundFile.number_of_channels == 1) {
      uint8_t samples[sampleLength];
      fread(&samples, sizeof(samples), 1, file);  //read sound info into an int array

      for (int i = 0, j = sampleLength - 1; i < j; i++, j--) {  //reverse the array
           uint8_t temp = samples[i];
           samples[i] = samples[j];
           samples[j] = temp;
        }

      fseek(file, sizeof(wavHeader), SEEK_SET); //point the file back to beginning
      fwrite(&samples, sizeof(samples), 1, file); //write changes to file
  }

  if((soundFile.bits_per_sample == 16 && soundFile.number_of_channels == 1) ||
     (soundFile.bits_per_sample == 8  && soundFile.number_of_channels == 2)) {
      uint16_t samples[sampleLength];
      fread(&samples, sizeof(samples), 1, file);

      for (int i = 0, j = sampleLength - 1; i < j; i++, j--) {
           uint16_t temp = samples[i];
           samples[i] = samples[j];
           samples[j] = temp;
        }

        fseek(file, sizeof(wavHeader), SEEK_SET);
        fwrite(&samples, sizeof(samples), 1, file);
  }

  if(soundFile.bits_per_sample == 16 && soundFile.number_of_channels == 2) {
      uint32_t samples[sampleLength];
      fread(&samples, sizeof(samples), 1, file);

      for (int i = 0, j = sampleLength - 1; i < j; i++, j--) {
           uint32_t temp = samples[i];
           samples[i] = samples[j];
           samples[j] = temp;
        }

        fseek(file, sizeof(wavHeader), SEEK_SET);
        fwrite(&samples, sizeof(samples), 1, file);
    }

  fclose(file); //close file and save changes
}

void readFile(char* filename, wavHeader* soundFile) {

  FILE* file = fopen(filename, "rb"); //open file in readBinary mode
  fread(soundFile, sizeof(*soundFile), 1, file);
  fclose(file);
}

int main(int argc, char** argv) {

  const char* REV = "-reverse"; //constants to store usage tags
  const char* RATE = "-rate";

  wavHeader soundFile;  //struct to store file header info

  if (argc == 1)
      showMenu();
  else if (argc == 2) {
    readFile(argv[1], &soundFile);
    showFileInfo(soundFile, argv[1] );
   }
  else if (argc == 3) { // -reverse
    if (streq_nocase(argv[2], REV) == 1)
        reverse(argv[1]);
    else {
        printf("Command not recognized. Displaying help menu...");
        showMenu();
        return 1;
      }
  } else {  //argc == 4, -rate -[INT]

      if (streq_nocase(argv[2], RATE) == 1)
          changeSampleRate(argv[1], argv[3]);
      else {
            printf("Command not recognized. Displaying help menu...");
            showMenu();
            return 1;
      }
    }
    return 0;
}
