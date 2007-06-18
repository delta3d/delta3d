#ifndef DELTA_EXAMPLE_CLIENTMESSAGEPROCESSOR
#define DELTA_EXAMPLE_CLIENTMESSAGEPROCESSOR

#include <dtGame/defaultmessageprocessor.h>

class ClientMessageProcessor : public dtGame::DefaultMessageProcessor
{
public:
	ClientMessageProcessor();
	virtual ~ClientMessageProcessor();

	void ProcessMessage(const dtGame::Message& msg);
};

#endif // DELTA_EXAMPLE_CLIENTMESSAGEPROCESSOR
