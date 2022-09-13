#include <screamdeck.h>
#include <stdio.h>
#include <stdlib.h>
#include <turbojpeg.h>

int main(int argc, char* argv[])
{
	scdk_device_t device;
	unsigned char* buffer;

	scdk_device_info_t* devices = scdk_enumerate();
	scdk_free_enumeration(devices);

	scdk_open(&device, NULL);
	if (device == NULL)
		return -1;

	buffer = malloc(SCDK_IMAGE_WIDTH * SCDK_IMAGE_HEIGHT * 3);
	if (buffer == NULL)
		return -1;

	tjhandle handle = tjInitDecompress();


	unsigned long file_length;
	FILE* file = fopen("../../../example/test.jpg", "rb");
	if (!file)
		return -1;

	fseek(file, 0, SEEK_END);
	file_length = ftell(file);
	fseek(file, 0, SEEK_SET);
	unsigned char* file_buffer = malloc(file_length + 1);
	if (file_buffer == NULL)
		return -1;

	fread(file_buffer, file_length, 1, file);
	fclose(file);


	int result = tjDecompress2(handle, file_buffer, file_length, buffer,
		SCDK_IMAGE_WIDTH, SCDK_IMAGE_WIDTH * 3, SCDK_IMAGE_HEIGHT, TJPF_RGB, 0);

	fclose(file);

	wchar_t serial_number[32];
	scdk_get_serial_number(device, serial_number, 32);

	scdk_set_brightness(device, 100);

	scdk_set_image(device, buffer, SCDK_PIXEL_FORMAT_RGB, 100);

	scdk_set_image(device, buffer, SCDK_PIXEL_FORMAT_RGB, 100);
	
	bool keys[SCDK_KEY_GRID_WIDTH * SCDK_KEY_GRID_HEIGHT] = {0};
	while(true)
	{
		bool keys_buffer[SCDK_KEY_GRID_WIDTH * SCDK_KEY_GRID_HEIGHT] = {0};
		const int result = scdk_read_key_timeout(device, keys_buffer, SCDK_KEY_GRID_WIDTH * SCDK_KEY_GRID_HEIGHT, 1000 / 60);
		if (result == -1)
		{
			break;
		}
		else if (result > 0)
		{
			for(int i = 0; i < SCDK_KEY_GRID_WIDTH * SCDK_KEY_GRID_HEIGHT; ++i)
			{
				if (keys[i] != keys_buffer[i])
				{
					if (keys[i])
						printf("Key %d down\n", i);
					else
						printf("Key %d up\n", i);

					keys[i] = keys_buffer[i];
				}
			}
		}
	}

	scdk_free(device);

	free(buffer);

	return 0;
}
