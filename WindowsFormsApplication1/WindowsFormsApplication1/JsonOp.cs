using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WindowsFormsApplication1
{
    
    public
    class JsonOp
    {
        const int iHead = 12;

        public
       static void PacketToSend(UInt32 iCLientNo, int iFunNo, string jsonText, out string strPacket)
        {
            char[] bb = new char[iHead];
            bb[0] = 'W';
            bb[1] = 'J';

            bb[2] = (char)((iCLientNo & 0xff000000) >> 24);
            bb[3] = (char)((iCLientNo & 0xff0000) >> 16);
            bb[4] = (char)((iCLientNo & 0xff00) >> 8);
            bb[5] = (char)((iCLientNo & 0xff));
            bb[6] = (char)0;

            bb[7] = (char)(iFunNo);

            int iLen = jsonText.Length;
            bb[8] = (char)((iLen & 0xff000000) >> 24);
            bb[9] = (char)((iLen & 0xff0000) >> 16);
            bb[10] = (char)((iLen & 0xff00) >> 8);
            bb[11] = (char)((iLen & 0xff));

            
            string Head = new string(bb);

            int ilen = Head.Length;
            ilen = jsonText.Length;
            strPacket = Head + jsonText;
            ilen = strPacket.Length;
        }


        public
        static bool UnPacketHead(string StrRecv, out int iFuncNo, out int iClientNo)
        {
            if (StrRecv.Length >= iHead && StrRecv[0] == 'W' && StrRecv[1] == 'J')
            {
                int i32No = StrRecv[2] << 24;
                 int i24No = StrRecv[3] << 16;
                int i16NO = StrRecv[4] << 8;
                int i8NO= StrRecv[5];
                iClientNo = i32No + i24No + i16NO + i8NO ;

                iFuncNo = StrRecv[7];

              
                return true;
            }
            iFuncNo = -1;
            iClientNo = -1;
            return false;
        }

        public
      static void PacketLogOnInfo(string username, string pwd, out string jsonText)
        {
            try
            {
                StringWriter textWriter = new StringWriter();

                JsonTextWriter jsonWriter = new JsonTextWriter(textWriter);
                jsonWriter.WriteStartObject();
                jsonWriter.WritePropertyName("username");
                jsonWriter.WriteValue(username);
                jsonWriter.WritePropertyName("pwd");
                jsonWriter.WriteValue(pwd);
                jsonWriter.WriteEndObject();
                jsonWriter.Flush();
                jsonText = textWriter.GetStringBuilder().ToString();
            }
            catch(Exception dd)
            {
                jsonText = "";
            }

           
        }

        public
      static bool UnpacketResult(string StrRecv, out string errStr)
        {
            bool bRet = false;
            try
            {
                JObject jo = (JObject)JsonConvert.DeserializeObject(StrRecv.Substring(iHead));
                string zone = jo["result"].ToString();
                errStr = jo["error"].ToString();
                if ("True" == zone)
                    bRet = true;
            }
            catch (Exception ee)
            {
                errStr = "";
            }

           
            return bRet;
        }

        public
        static void PacketConnectString(string sqluser, string sqlpwd, string dbname, int dbtype, out string strBody)
        {
            try
            {
                StringWriter textWriter = new StringWriter();

                JsonTextWriter jsonWriter = new JsonTextWriter(textWriter);
                jsonWriter.WriteStartObject();
                jsonWriter.WritePropertyName("username");
                jsonWriter.WriteValue(sqluser);
                jsonWriter.WritePropertyName("pwd");
                jsonWriter.WriteValue(sqlpwd);
                jsonWriter.WritePropertyName("dbname");
                jsonWriter.WriteValue(dbname);
                jsonWriter.WritePropertyName("dbtype");
                jsonWriter.WriteValue(dbtype);
                jsonWriter.WriteEndObject();
                jsonWriter.Flush();
                strBody = textWriter.GetStringBuilder().ToString();
            }
            catch (Exception dd)
            {
                strBody = "";
            }
        }

        public
            static void PacketSelectString(string sqlRun, int icols, out string strBody)
        {
            try
            {
                //StringWriter textWriter = new StringWriter();

                //JsonTextWriter jsonWriter = new JsonTextWriter(textWriter);
                //jsonWriter.WriteStartObject();
                //jsonWriter.WritePropertyName("sqlcmd");
                //jsonWriter.WriteValue(sqlRun);
                //jsonWriter.WritePropertyName("array");
                //jsonWriter.WriteStartArray
                //jsonWriter.WriteValue(sqlpwd);
               
                //jsonWriter.WriteEndObject();
                //jsonWriter.Flush();
                //strBody = textWriter.GetStringBuilder().ToString();

                JObject root = new JObject();
                root["sqlcmd"] = sqlRun;
              
                //JArray array = new JArray();
                //foreach (string temp in cols)
                //{
                //    JObject obj = new JObject();
                //    obj["name"] = temp;
                //    array.Add(obj);
                //}
                root["count"] = icols;

                strBody = root.ToString();
                
            }
            catch (Exception dd)
            {
                strBody = "";
            }
        }


        internal static bool UnpacketDBdata(string sdata, out List<List<string>> lstdata, out string errStr)
        {
            bool bRet = false;
            lstdata = new List<List<string>>();
            try
            {
                string body = sdata.Substring(iHead);

                JObject jo = (JObject)JsonConvert.DeserializeObject(body);
                //JArray jArr = jo["data"];
               
                JArray jArr = JArray.Parse(jo["data"].ToString());

               
                for (int i = 0; i < jArr.Count; i++)
                {
                    JObject jtemp = JObject.Parse(jArr[i].ToString());

                    
                        List<string> lstTemp = new List<string>();
                        for (int j = 0; j < jtemp.Count; j++)
                        {
                            string ff = Convert.ToString(j);
                            string strCol = jtemp[ff].ToString();
                            lstTemp.Add(strCol);
                        }
                        lstdata.Add(lstTemp);
                        bRet = true;
                   
                   
                    
                   
                }

                errStr = jo["error"].ToString();
            }
            catch (Exception ee)
            {
                errStr = "";
            }

            
            return bRet;
        }
    }
}
