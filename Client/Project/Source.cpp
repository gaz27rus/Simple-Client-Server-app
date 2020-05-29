#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class Client
{
public:
	Client(const char* ip_adress, u_short port, ADDRESS_FAMILY protocol)
	{
		inet_pton(protocol, ip_adress, &(server.sin_addr));
		server.sin_port = htons(port);
		server.sin_family = protocol;
	}

	void connect_to_server()
	{
		Server = socket(server.sin_family, SOCK_STREAM, NULL);
		if (connect(Server, (SOCKADDR*)&server, sizeof(server)) != 0) {
			std::cout << "Error: failed connect to server.\n";
			exit(1);
		}
		std::cout << "Connected!\n";
	}

	void get_vector_from_file(const char *f_name)
	{
		std::ifstream vec(f_name);
		std::string a;

		while (getline(vec, a))
		{
			data.push_back(a);
		}
		vec.close();
	}

	void send_data()
	{
		for (int i = 0; i < data.size(); i++)
		{
			send(Server, data[i].c_str(), strlen(data[i].c_str()), 0);
			while (ready_to_send[0] == '0')
			{
				recv(Server, ready_to_send, 1, 0);
				if (ready_to_send[0] == '2') EXIT = true;
			}
			ready_to_send[0] = '0';
			if (EXIT) break;
		}
	}

	void receive_message()
	{
		std::string Buf;
		char tempBuf[256] = "";
		while (Buf.empty())
		{
			recv(Server, tempBuf, 256, 0);
			Buf = tempBuf;
		}
		std::cout << Buf;
	}

private:
	SOCKADDR_IN server;
	SOCKET Server;
	std::vector<std::string> data;
	char ready_to_send[1] = { '0' };
	char stop_receiving[1] = { '2' };
	bool EXIT = false;
};

void init_library()
{
	WSAData wsaData;
	WORD DLLVersion = MAKEWORD(2, 1);
	if (WSAStartup(DLLVersion, &wsaData) != 0)
	{
		std::cout << "Error" << std::endl;
		exit(1);
	}
}

int main(int argc, char* argv[])
{
	init_library();

	Client client1("127.0.0.1", 1000, AF_INET);
	client1.connect_to_server();
	client1.get_vector_from_file("1.txt");
	client1.send_data();
	client1.receive_message();

	system("pause");
	return 0;
}