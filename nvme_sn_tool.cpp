#include "nvme_util.h"
#include <iostream>
#include <unistd.h>
#include <vector>

using namespace std;

int getNVMEIDInfo(std::string deviceStr);
const char * format_char_array(char * str, int strsize, const char * chr, int chrsize);
template<size_t STRSIZE, size_t CHRSIZE>
inline const char * format_char_array(char (& str)[STRSIZE], const char (& chr)[CHRSIZE])
  { return format_char_array(str, (int)STRSIZE, chr, (int)CHRSIZE); }

const char *format_char_array(char *str, int strsize, const char *chr, int chrsize)
{
	int b = 0;
	while (b < chrsize && chr[b] == ' ')
		b++;
	int n = 0;
	while (b + n < chrsize && chr[b + n])
		n++;
	while (n > 0 && chr[b + n - 1] == ' ')
		n--;

	if (n >= strsize)
		n = strsize - 1;

	for (int i = 0; i < n; i++)
	{
		char c = chr[b + i];
		str[i] = (' ' <= c && c <= '~' ? c : '?');
	}
	str[n] = 0;
	return str;
}

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) {
            result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int getNVMEIDInfo(string deviceStr)
{
	nvme_Device * nvmeDev;
	nvmeDev = new nvme_Device(deviceStr.c_str(), "", 0);

	int fd = nvmeDev->nvmeOpen();
	if (fd < 0)
		return -1;

	char buf[64];
	nvme_Device::nvme_id_ctrl id_ctrl;
	const char *model_str_byte;
	const char *serialno_str_byte;
	const char *fwrev_str_byte;

	nvmeDev->nvme_read_id_ctrl(id_ctrl);
	cout << "---------- Disk Information ----------" << endl;
	
	model_str_byte = format_char_array(buf, id_ctrl.mn);
	cout << "Model\t\t\t: " + std::string(model_str_byte) << endl;
	
	fwrev_str_byte = format_char_array(buf, id_ctrl.fr);
	cout << "FW Version\t\t: " + std::string(fwrev_str_byte) << endl;

	serialno_str_byte = format_char_array(buf, id_ctrl.sn);
	cout << "Serial No\t\t: " + std::string(serialno_str_byte) << endl;

	::close(fd);

	return 0;
}

void showGuide()
{
	cout << "Usage:" << endl;
	cout << " " << "nvme_sn_tool <nvme_device>" << endl;
}

int main(int argc, char *argv[])
{
	std::vector<std::string > args;
	for (int i = 0; i < argc; i++)
	{
		args.push_back(argv[i]);
	}

	if (argc == 2){
		if (args[1].find("nvme") != std::string::npos)
		{
			int resultGetDevice = getNVMEIDInfo(args[1]);
			if(resultGetDevice == -1){
				cout << args[1] << " Device not found. " << endl;
			}
		}
		else{
			showGuide();
		}
	}
	else{
		showGuide();
	}

}
