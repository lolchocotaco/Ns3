

class App: public Application{
    private:
        bool isRunning; 
        DataRate dataRate;
        uint32_t numPacketSent;
        uint32_t numPackets;
        uint32_t packetSize;
        Socket * socket;
        Address peer;
        EventId sendEvent;

        void StartApplication();
        void StopApplication();

        void ScheduleTx();
        void sendPacket();
    public:
        App();
        ~App();        
        void Setup(Socket* socket, Address address, uint32_t packetSize, uint32_t numPackets, DataRate dataRate);
        void ChangeRate( DataRate newRate);
};


App::App() :
    socket(0),
    dataRate(0),
    numPacketSent(0),
    numPacket(0),
    packetSize(0),
    socket(0),
    peer(),
    sendEvent()
{
}

App:: ~App(){
    socket = 0;
}

void App::Setup (Socket* socket, Address address, uint32_t packetSize, uint32_t numPackets, DataRate dataRate){
    this->socket = socket;
    this->address = address;
    this->packetSize = packetSize;
    this->numPackets = numPackets;
    this->DataRate = dataRate;
}

void App::StartApplication(){
    running=true;
    numPacketSent = 0;
    socket->Bind ();
    socket->Connect(peer);
    SendPacket();
}

void App::StopApplication(){
    running = false;
    if(sendEvent.IsRunning() ){
        Simulator::Cancel(sendEvent);
    }
    if(socket){
        socket->Close();
    }
}

void App::SendPacket(){
    Packet* packet = Create<Packet>(packetSize);
    socket->Send(packet);
    if(++numPacketSent<numPackets){
        ScheduleTx();
    }

}

void App::ScheduleTx(){
    if(running){
        Time tNext (Seconds( packetSize *8 / static_cast<double> (dataRate.GetBitRate() ) ) ) ;
        sendEvent = Simulator::Schedule(tNext,&App::SendPacket, this);
    }
}

void App::ChangeRate(DataRate newRate){
    dataRate = newRate
    return;
}


