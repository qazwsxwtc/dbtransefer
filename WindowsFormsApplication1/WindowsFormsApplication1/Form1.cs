using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    enum enumFunctionNo
    {
        LogIn = 0,
        ConnectSql = 1,
        ExcSelectCmd = 2,
    };

    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();

           
            //Thread t1 = new Thread(new ThreadStart(TestMethod));
            //t1.Start();
            //TcpConnet pff = new TcpConnet();
            //pff.ConnectToServer();
            //TcpConnet::Get_TcpConnet();

           // TcpConnet.Get_TcpConnet().SendData("abccc");
        }

        private void logon_Click(object sender, EventArgs e)
        {
            //TcpConnet.Get_TcpConnet().SendData("abccc");
            string strBody;
            string strPacket;
            string strUser = "admin";
            string strPwd = "admin";
            JsonOp.PacketLogOnInfo(strUser, strPwd, out strBody);
            JsonOp.PacketToSend(0, (int)enumFunctionNo.LogIn, strBody, out strPacket);
            TcpConnet.Get_TcpConnet().SendData(strPacket);

           // TcpConnet.Get_TcpConnet().SendData("00005");
        }

        private void ConnectDb_Click(object sender, EventArgs e)
        {
            string dbname = "sql2012";
            string username = "geomos_admin2";
            string pwd = "geomos_admin_2006";
            string strBody;
            string strPacket;
            JsonOp.PacketConnectString(username, pwd, dbname, 1, out strBody);
            uint nn = TcpConnet.Get_TcpConnet().getClientNo();
            JsonOp.PacketToSend(nn, (int)enumFunctionNo.ConnectSql, strBody, out strPacket);
            TcpConnet.Get_TcpConnet().SendData(strPacket);
        }

        private void ReadDb_Click(object sender, EventArgs e)
        {
            string strSql = "SELECT [ID], [Name] FROM  Points";
            int iCol = 2;
            //cols.Add("1");
            //cols.Add("2");
            string strBody, strPacket;
            JsonOp.PacketSelectString(strSql, iCol, out strBody);
             uint nn = TcpConnet.Get_TcpConnet().getClientNo();
            JsonOp.PacketToSend(nn, (int)enumFunctionNo.ExcSelectCmd, strBody, out strPacket);
             TcpConnet.Get_TcpConnet().SendData(strPacket);
        }

        private delegate void DispMSGDelegate(string MSG);



        public void DispMsg(string strMsg)
        {
            if (this.richTextBox1.InvokeRequired == false)                      //如果调用该函数的线程和控件lstMain位于同一个线程内
              {
                  ////直接将内容添加到窗体的控件上
                  //ListViewItem lvi = new ListViewItem();
                  //lvi.SubItems.Add(strMsg);
                  //this.listBox1.Items.Insert(0, lvi);
                //richTextBox1
              }
              else                                                        //如果调用该函数的线程和控件lstMain不在同一个线程
              {
                  //通过使用Invoke的方法，让子线程告诉窗体线程来完成相应的控件操作
                  //DispMSGDelegate DMSGD = new DispMSGDelegate(DispMsg);
  
                  ////使用控件lstMain的Invoke方法执行DMSGD代理(其类型是DispMSGDelegate)
                  //this.richTextBox1.Invoke(DMSGD, strMsg);


                  this.BeginInvoke(new Action<string>((x) => { richTextBox1.Text += "\n" + x.ToString(); }), strMsg);

                 
              }
         }


        private void button1_export(object sender, EventArgs e)
        {


            new Thread(SecletThread).Start();

            new Thread(RecvThread).Start();
            
            
        }

        private void SecletThread(object obj)
        {
            string strSql = "select * from Table_ee";
            int icold = 2;
            string strBody, strPacket;
            JsonOp.PacketSelectString(strSql, icold, out strBody);
            uint nn = TcpConnet.Get_TcpConnet().getClientNo();
            JsonOp.PacketToSend(nn, (int)enumFunctionNo.ExcSelectCmd, strBody, out strPacket);
            TcpConnet.Get_TcpConnet().SendData(strPacket);

            Thread.Sleep(1000*60*5);//每5分钟发一次查询命令
        }

        private void RecvThread(object obj)
        {
            while (true)
            {
                CData odata = new CData();
                bool bOk = DataMgr.Get_DataMgr().Getdata(out odata);
                if (bOk)
                {
                    List<List<string>> ff = odata.m_lstdata;
                    List<string> oCols = odata.m_colName;
                    //进行数据处理
                }
                Thread.Sleep(3000);//每3秒钟检测一次内存缓冲区
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            int iPort = 15555;
            string ipAddr = "47.99.149.104";
            TcpConnet.Get_TcpConnet().ConnectToServer(iPort, ipAddr);
            TcpConnet.Get_TcpConnet().addForm(this);
        }

    }
}
