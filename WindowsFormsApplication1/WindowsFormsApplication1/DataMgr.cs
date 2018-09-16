using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WindowsFormsApplication1
{

    public
    struct RecvDataStruct
    {
        public int ifd ;
        public uint itotallen ;
        public uint iRemainlen;
        public string strrecvData;
    }


    public
    struct CData
    {
       public List<List<string>> m_lstdata;
       public List<string> m_colName;
    }

    public
    class DataMgr
    {
        public
       static DataMgr Get_DataMgr()
        {
            if (m_DataMgr == null)
            {
                m_DataMgr = new DataMgr();
            }
            return m_DataMgr;
        }

        public bool Getdata(out CData oData)
        {
            oData = new CData();
            lock(m_lst)
            {
                if(null != m_lst && m_lst.Count > 0)
                {
                    oData = m_lst[0];
                    m_lst.RemoveAt(0);
                    return true;
                }
                else
                {
                    return false;
                }
                    
            }
        }

        public void AddData(CData oData)
        {
            lock(m_lst)
            {
                m_lst.Add(oData);
            }
        }


        public List<RecvDataStruct> Get()
        {
            return m_RecvLst;
        }

        public void Set(RecvDataStruct oo)
        {
            m_RecvLst.Clear();
            m_RecvLst.Add(oo);
        }

        internal void Remove()
        {
            m_RecvLst.Clear();
        }

        private List<CData> m_lst = new List<CData>();

        private static DataMgr m_DataMgr = null;


        public List< RecvDataStruct> m_RecvLst = new List<RecvDataStruct>();

      
    }
}
