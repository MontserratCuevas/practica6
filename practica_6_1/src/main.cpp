#include <Arduino.h>
//bool SDFS::begin(uint8_t ssPin, SPIClass &spi, uint32_t frequency, const char * mountpoint, uint8_t max_files, bool format_if_empty)
//    void begin(int8_t sck=-1, int8_t miso=-1, int8_t mosi=-1, int8_t ss=-1);
//  1000, "/sd", 5 ,true 

#include <SPI.h>
#include <SD.h>
File myFile;
//File file1;

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\r\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("- failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("- file written");
    } else {
        Serial.println("- write failed");
    }
    file.close();
}

void setup()
{
  Serial.begin(115200);
  SPI.begin(36, 37, 35);
  Serial.print("Iniciando SD ...");
  if (!SD.begin(39, SPI)) {
    Serial.println("No se pudo inicializar");
  return;
  }

  Serial.println("inicializacion exitosa");
  writeFile(SD, "/hello1.txt", "Hello world 1, PD"); // Create a hello1.txt file with the content "Hello1"

  myFile = SD.open("/arxivo.txt");//abrimos el archivo 
  if (myFile) {
    Serial.println("arxivo.txt:");
  while (myFile.available()) {
    Serial.write(myFile.read());
  }
  myFile.close(); //cerramos el archivo
  } else {
    Serial.println("Error al abrir el arxivo");
  }
}
void loop()
{
}

