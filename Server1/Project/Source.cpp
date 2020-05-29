#pragma comment(lib, "ws2_32.lib")
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <vector>
#include <string>

bool is_digit(std::string str)
{
	int k = 0;
	for (int i = 0; i < str.length(); i++)
	{
		if (isdigit(str[i]))
		{
			k++;
		}
	}
	if (k == str.length()) return true; else return false;
}

int str_to_int(std::string str)
{
	int num = 0;
	int power = str.length() - 1;
	for (int i = 0; i < str.length(); i++, power--)
	{
		num += ((int(str[i]) - 48) * pow(10, power));
	}
	return num;
}

void clear_buf(char *buf, int size)
{
	memset(buf, 0, size * (sizeof buf[0]));
}

class Server
{
public:
	Server(const char* ip_adress, u_short port, ADDRESS_FAMILY protocol)
	{
		inet_pton(protocol, ip_adress, &(server.sin_addr));
		server.sin_port = htons(port);
		server.sin_family = protocol;
	}

	void start_listening(int backlog)
	{
		Listen = socket(server.sin_family, SOCK_STREAM, NULL);
		bind(Listen, (SOCKADDR*)&server, sizeof(server));
		listen(Listen, backlog);
	}

	void accept_client()
	{
		int size = sizeof(server);
		Client = accept(Listen, (SOCKADDR*)&server, &size);
		if (Client == 0) 
		{
			std::cout << "Error #2\n";
		}
		else 
		{
			std::cout << "Client Connected!\n";
		}
	}

	void receive_data()
	{
		std::string Buf;
		char tempBuf[256] = "";
		while (true)
		{
			int BytesRecv = recv(Client, tempBuf, 256, 0);

			if (strcmp("", tempBuf))
				Buf = tempBuf;
			

			if (!is_digit(Buf))
			{
				send(Client, stop_receiving, 1, 0);
				break;
			}
			else data.push_back(Buf);
			clear_buf(tempBuf, 256);
			send(Client, ready_to_receive, 1, 0);
		}
	}

	int processing_data()
	{
		int sum = 0;
		for (int i = 0; i < data.size(); i++)
		{
			sum += str_to_int(data[i]);
		}
		return sum;
	}

	void send_message(std::string message)
	{
		send(Client, message.c_str(), message.length(), 0);
	}

private:
	SOCKADDR_IN server;
	SOCKET Listen;
	SOCKET Client;
	std::vector<std::string> data;
	char ready_to_receive[1] = { '1' };
	char stop_receiving[1] = { '2' };
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

	Server server1("127.0.0.1", 1000, AF_INET);
	server1.start_listening(SOMAXCONN);
	server1.accept_client();
	server1.receive_data();
	int sum = server1.processing_data();
	server1.send_message(std::to_string(sum));

	system("pause");
	return 0;
}