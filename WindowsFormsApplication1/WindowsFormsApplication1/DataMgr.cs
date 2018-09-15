using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WindowsFormsApplication1
{
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

        private List<CData> m_lst = new List<CData>();

        private static DataMgr m_DataMgr = null;
    }
}
