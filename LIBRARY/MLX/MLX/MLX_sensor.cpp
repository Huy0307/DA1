#include "MLX_sensor.h"
Adafruit_MLX90614 mlx = Adafruit_MLX90614();
void MLXsetup(){
  mlx.begin();
}
void readTemperature(float &object_temp, float &ambient_temp) {
  object_temp = mlx.readObjectTempC();
  ambient_temp = mlx.readAmbientTempC();
}
