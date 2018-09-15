using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Threading; 

namespace WindowsFormsApplication1
{

   

    public
    class TcpConnet
    {
        internal class StateObject
        {
            public TcpClient client = null;
            public int totalBytesRead = 0;
            public const int BufferSize = 1024*1024*10;
            public string readType = null;
            public byte[] buffer = new byte[BufferSize];
            public StringBuilder messageBuffer = new StringBuilder();
        }
        
        public
        void ConnectToServer(int port, string host)
        {
            //IPAddress ip = IPAddress.Parse(host);
            //IPEndPoint ipe = new IPEndPoint(ip, port);

            //clientSocket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            //clientSocket.Connect(ipe);

            if ((null == tcpCLient) || (!tcpCLient.Connected))
            {
                try
                {
                    tcpCLient = new TcpClient();
                    tcpCLient.ReceiveTimeout = 15;

                    connectDone.Reset();
                    tcpCLient.BeginConnect(host, port, new AsyncCallback(ConnectCallback), tcpCLient);
                    connectDone.WaitOne();
                    if ((tcpCLient != null) && (tcpCLient.Connected))
                    {
                        newWorkStream = tcpCLient.GetStream();
                        asyncread(tcpCLient);
                    }

                }
                catch (Exception ss)
                {

                }
            }  
        }

        /// <summary>

        /// 异步连接的回调函数

        /// </summary>

        /// <param name="ar"></param>

        private void ConnectCallback(IAsyncResult ar)
        {
            connectDone.Set();
            TcpClient t = (TcpClient)ar.AsyncState;
            try
            {
                if (t.Connected)
                {                
                    t.EndConnect(ar);
                }
                else
                {
                    t.EndConnect(ar);
                }
                
            }
            catch (SocketException se)
            {
            }
        }


        /// <summary>

        /// 断开连接

        /// </summary>

        private void DisConnect()
        {
            if ((tcpCLient != null) && (tcpCLient.Connected))
            {
                newWorkStream.Close();
                tcpCLient.Close();
            }
        }

        public
        void SendData(string sendStr)
        { //send message
         
         

            char[] szChar = sendStr.ToCharArray();
            byte[] sendBytes = new byte[szChar.Length];//Encoding.ASCII.GetBytes(sendStr);

            for(int i = 0; i< szChar.Length; i++)
            {
                sendBytes[i] = (byte)szChar[i];
            }

          

            if (newWorkStream != null && newWorkStream.CanWrite)
            {
                newWorkStream.Write(sendBytes, 0, sendBytes.Length);
            }
        }

    
   

        public
        static TcpConnet Get_TcpConnet()
        {
            if (m_TcpConnet == null)
            {
                m_TcpConnet = new TcpConnet();
            }
            return m_TcpConnet;
        }



        /// <summary>

        /// 异步读TCP数据

        /// </summary>

        /// <param name="sock"></param>

        private void asyncread(TcpClient sock)
        {
            StateObject state = new StateObject();
            state.client = sock;
            NetworkStream stream = sock.GetStream();

            if (stream.CanRead)
            {
                try
                {
                    IAsyncResult ar = stream.BeginRead(state.buffer, 0, StateObject.BufferSize,
                            new AsyncCallback(TCPReadCallBack), state);
                }
                catch (Exception e)
                {
                    
                }
            }
        }

        /// <summary>

        /// TCP读数据的回调函数

        /// </summary>

        /// <param name="ar"></param>

        private void TCPReadCallBack(IAsyncResult ar)
        {
            StateObject state = (StateObject)ar.AsyncState;
            //主动断开时

            if ((state.client == null) || (!state.client.Connected))
                return;
            int numberOfBytesRead = 0;
            NetworkStream mas = state.client.GetStream();
            string type = null;
            try
            {
                numberOfBytesRead = mas.EndRead(ar);
            }
            catch(Exception dd)
            {

            }
           
            state.totalBytesRead += numberOfBytesRead;

           // string strRev = numberOfBytesRead.ToString();
            if (numberOfBytesRead > 0)
            {
                byte[] dd = new byte[numberOfBytesRead];
                Array.Copy(state.buffer,0,dd,0,numberOfBytesRead);
              //  OnGetData(dd);
                char [] szchar = new char[dd.Length];
                for(int i =0; i< dd.Length; i++)
                {
                    szchar[i] = (char)dd[i];
                }

                string sdata = new string(szchar);

                OnRecvData(sdata);
                mas.BeginRead(state.buffer, 0, StateObject.BufferSize,
                        new AsyncCallback(TCPReadCallBack), state);
            }
            else
            {
                //被动断开时 

                mas.Close();
                state.client.Close();
                m_Form1.DispMsg("connect close");
                mas = null;
                state = null;

               
            }
        }

        private void OnRecvData(string sdata)
        {
            if (sdata.Length < 12 && sdata == "login")
           {

           }
            else if (sdata.Length >= 12 && sdata[0] == 'W' && sdata[1] == 'J')
            {
                int iClientNo;
                int iFuncNo;
                if(JsonOp.UnPacketHead(sdata, out iFuncNo, out iClientNo))
                {
                    switch(iFuncNo)
                    {
                        case (int)enumFunctionNo.LogIn:
                            {
                                string errStr;
                                if (JsonOp.UnpacketResult(sdata, out errStr))
                                {
                                    m_iClientNo = (uint)iClientNo;
                                    m_Form1.DispMsg("enumFunctionNo.LogIn ok :" + errStr);

                                }
                                else 
                                {
                                    m_Form1.DispMsg("enumFunctionNo.LogIn error");
                                }
                            }
                            break;

                        case (int)enumFunctionNo.ConnectSql:
                            {
                                string errStr;
                                if (JsonOp.UnpacketResult(sdata, out errStr))
                                {
                                    m_iClientNo = (uint)iClientNo;
                                    m_Form1.DispMsg("enumFunctionNo.ConnectSql ok: " + errStr);
                                }
                                else
                                {
                                    //error tips
                                    m_Form1.DispMsg("enumFunctionNo.ConnectSql error: " + errStr);
                                }
                            }
                            break;
                        case (int)enumFunctionNo.ExcSelectCmd:
                            {
                                if (sdata.Length == 12)
                                {
                                    m_Form1.DispMsg("enumFunctionNo.ExcSelectCmd empty data");
                                }
                                else 
                                {
                                  
                                    List<List<string>> lstdata;// = new List<List<string>>();
                                    string errStr;
                                    JsonOp.UnpacketDBdata(sdata, out lstdata, out errStr);
                                    if (lstdata.Count > 0)
                                    {
                                        CData odata = new CData();
                                        odata.m_lstdata = lstdata;
                                        DataMgr.Get_DataMgr().AddData(odata);

                                        m_Form1.DispMsg("enumFunctionNo.ExcSelectCmd get  data sucee");
                                    }     
                                    else
                                    {
                                        m_Form1.DispMsg("enumFunctionNo.ExcSelectCmd get  data error:" + errStr);
                                    }
                                }
                                
                            }
                            break;

                        default:
                            break;
                    }
                }
            }
        }

   
        public
            uint getClientNo()
        {
           return m_iClientNo;
        }
    
       

       private TcpClient tcpCLient = null;
       private static TcpConnet m_TcpConnet = null;

       private NetworkStream newWorkStream = null;


       public uint m_iClientNo = 0;
       public ManualResetEvent connectDone = new ManualResetEvent(false);

       private Form1 m_Form1 = null;
       internal void addForm(Form1 form1)
       {
           m_Form1 = form1;
       }
    }
}
