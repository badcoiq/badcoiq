class bqFrameworkCallbackCB : public bqFrameworkCallback
{
public:
    bqFrameworkCallbackCB() {}
    virtual ~bqFrameworkCallbackCB() {}
    virtual void OnMessage() {}
};

bool g_run = true;

int main()
{
    bqFrameworkCallbackCB fcb;
    bqFramework::Start(&fcb);

    while (g_run)
    {
        bqFramework::Update();
    }
               

    return EXIT_SUCCESS;
}
