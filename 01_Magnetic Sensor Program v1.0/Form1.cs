using MetroFramework.Forms;
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
using System.IO;
using System.IO.Ports;
using System.Runtime.InteropServices;
using Excel = Microsoft.Office.Interop.Excel;
using SENSOR_LOGGER;



namespace Wind_Monitoring_Program
{
    public partial class Form1 : MetroForm
    {

        static public readonly int BUFF_SIZE = 4096;
        SerialPort Road1_SerialPort = new SerialPort();
        //SensorValue _RoadSensorValue;
        byte[] procBuff = new byte[BUFF_SIZE];
        byte[] recvBuff = new byte[BUFF_SIZE];
        byte[] sendBuff = new byte[BUFF_SIZE];


        int recv_ptr = 0;
        int proc_ptr = 0;

        byte data = 0;


        string sPort = "";
        string sBaud = "";


        int proc_mode = 0;

        int proc_index = 0;
        byte proc_length = 0;

        private Thread _recvThread;

        public string threadMessage;
        public string recvMessage;
        //int recv_ptr = 0;
        public bool RequestCode = false;
        float currentHeading = 0.0f;
        float currentHeading_Slave = 0.0f;
        public string m_strPath = System.Environment.CurrentDirectory + "\\Config.ini"; //이전 시리얼 포트 설정 저장된 ini파일
        //double temp,humi,Otemp;
        delegate void LSetText(Label lb, string text);
        delegate void DSetText(RichTextBox rtb, string text, int opt);
        SensorValue _SensorValue;
        SensorValue _SensorValue_Slave;
        SensorValue _SensorValue_Final;

        readonly object _lock = new object();   // Used to protect shared data (serial thread safety)

        // --- Queues store time-ordered detection events ---
        Queue<DateTime> _evMaster = new Queue<DateTime>();   // Master sensor vehicle detection times
        Queue<DateTime> _evSlave = new Queue<DateTime>();   // Slave sensor vehicle detection times
        Queue<DateTime> _evMatched = new Queue<DateTime>();  // Successfully matched vehicle timestamps (for traffic/min)

        // --- Configuration values ---
        double _distanceMeters = 5.0;            // Sensor distance (m)
        const double MATCH_WINDOW_SEC = 5;     // Maximum time gap between Master/Slave to count as same vehicle
        const double MIN_TIME_SEC = 0.02;        // Reject unrealistically small time (too fast)
        const double MAX_TIME_SEC = 5.0;         // Reject unrealistically large time (too slow)

        // --- Calculated results ---
        double _lastSpeedKmh = 0.0;              // Latest computed vehicle speed (km/h)
        int _lastTrafficPerMin = 0;           // Number of matched vehicles in the last 60 seconds

        // --- Optional smoothing of speed values ---
        Queue<double> _recentSpeeds = new Queue<double>();
        const int SPEED_MEDIAN_N = 5;            // Median filter window size (use 3~5)

        private static readonly object _eventLogLock = new object();

        public Form1()
        {
            InitializeComponent();
            Road1_SerialPort.DataReceived += new SerialDataReceivedEventHandler(serial_DataReceived1);
            _SensorValue = new SensorValue();
            _SensorValue_Slave = new SensorValue();
            _SensorValue_Final = new SensorValue();
            COM_PORT_Init(cbSerialPort, cbSerialBaud);
            _recvThread = new Thread(Thread_Task);
            _recvThread.Start();
        }

        [DllImport("kernel32")]
        private static extern long WritePrivateProfileString(string section,
                                         string key, string val, string filePath);
        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section,
                                        string key, string def, StringBuilder retVal,
                                        int size, string filePath);
        private string GetProfileString(string section, string key, string def, int size, string filePath)
        {
            StringBuilder temp = new StringBuilder(255);
            int i = GetPrivateProfileString(section, key, def, temp, size, filePath);
            return temp.ToString();
        }

        delegate void LSetTextBox(TextBox tb, string text);
        delegate void LSetButtonText(Button tb, string text);
        delegate void LSetForeColor(Label lb, bool v);

        delegate void PBSetBackColor(PictureBox pb, Color value);
        public void SetBackColor(PictureBox pb, Color value)
        {
            if (pb.InvokeRequired)
            {
                PBSetBackColor d = new PBSetBackColor(SetBackColor);
                this.Invoke(d, pb, value);
            }
            else
            {
                pb.BackColor = value;
            }
        }

        public void SetLable(Label lb, string text)
        {
            if (lb.InvokeRequired)
            {
                LSetText d = new LSetText(SetLable);
                this.Invoke(d, lb, text);
            }
            else
            {
                lb.Text = text;
            }
        }
        void COM_PORT_Init(ComboBox cbPort, ComboBox cbBaud)
        {
            string[] ports = SerialPort.GetPortNames();
            cbSerialPort.Items.AddRange(ports);

            int i;

            sPort = GetProfileString("SERIAL", cbPort.Name, "COM3", 255, m_strPath);
            sBaud = GetProfileString("SERIAL", cbBaud.Name, "38400", 255, m_strPath);

            //Port
            cbPort.BeginUpdate();
            cbPort.Items.Clear();
            foreach (string comport in SerialPort.GetPortNames())
            {
                cbPort.Items.Add(comport);
            }
            cbPort.EndUpdate();

            i = cbPort.FindString(sPort);
            if (i >= 0)
            {
                cbPort.SelectedIndex = i;
            }

            i = cbBaud.FindString(sBaud);
            if (i >= 0)
            {
                cbBaud.SelectedIndex = i;
            }
        }

        private void serial_DataReceived1(object sender, SerialDataReceivedEventArgs e)
        {
            int len = Road1_SerialPort.BytesToRead;
            byte[] tempbuff = new byte[len];
            Road1_SerialPort.Read(tempbuff, 0, len);

            for (int i = 0; i < len; i++)
            {
                recvBuff[recv_ptr++] = tempbuff[i];
                recv_ptr %= BUFF_SIZE;
            }

/*            //this.Invoke(new EventHandler(SerialReceived1));
            try
            {
                if (pbRxDStatus.BackColor == Color.Orange)
                    pbRxDStatus.BackColor = Color.White;
                else
                    pbRxDStatus.BackColor = Color.Orange;


                int RecSize = Road1_SerialPort.BytesToRead;
                byte[] rx_buff = new byte[RecSize];
                byte[] rx_ch = new byte[1];
                Road1_SerialPort.Read(rx_buff, 0, RecSize);

                for (int i = 0; i < RecSize; i++)
                {
                    rx_ch[0] = rx_buff[i];
                    switch (proc_mode)
                    {
                        case 0:
                            proc_index = 0;
                            threadMessage = "";
                            if (rx_ch[0] == 0x7E)       //STX
                            {
                                procBuff[proc_index++] = rx_ch[0];
                                threadMessage += string.Format("[{0:X2}]", rx_ch[0]);
                                proc_mode++;
                            }
                            else
                            {
                                proc_index = 0;
                                proc_mode = 0;
                            }
                            break;
                        case 1:                     //len
                            procBuff[proc_index++] = rx_ch[0];
                            proc_length = rx_ch[0] - 1;
                            threadMessage += string.Format(" [{0:X2}]", rx_ch[0]); ;
                            proc_mode++;
                            break;
                        case 2:
                            procBuff[proc_index++] = rx_ch[0];
                            threadMessage += string.Format(" [{0:X2}]", rx_ch[0]);
                            proc_length--;
                            if (proc_length == 0)
                            {
                                proc_mode++;
                            }
                            break;
                        case 3:
                            if (rx_ch[0] == 0x7F)
                            {
                                procBuff[proc_index++] = rx_ch[0];
                                threadMessage += string.Format(" [{0:X2}]", rx_ch[0]);

                                recvMessage = threadMessage;

                                DataCheck(procBuff);

                                proc_mode = 0;
                            }
                            break;
                    }


                }
*//*                if (RecSize <= 0) return;
                if (rx_buff[RecSize - 1 ] == '\r')
                {
                    string str = Encoding.Default.GetString(rx_buff);
                    recvMessage = str;
                    SetText(tb_RecvMsg, recvMessage, 0);
                }
                else
                {


                    //SetText(tb_RecvMsg, recvMessage, 0);
                }*//*
            }
            catch { }*/


        }



        static public string ToReadableByteArray(byte[] bytes)
        {
            return string.Join(",", bytes);
        }

        void Thread_Task()
        {
            while (true)
            {
                if (recv_ptr != proc_ptr)
                {
                    data = recvBuff[proc_ptr++];
                    proc_ptr %= BUFF_SIZE;

                    switch (proc_mode)
                    {
                        case 0:
                            proc_index = 0;
                            threadMessage = "";
                            if (data == 0x7E)       //STX
                            {
                                procBuff[proc_index++] = data;
                                threadMessage += string.Format("[{0:X2}]", data);
                                proc_mode=2;
                            }
                            else
                            {
                                proc_index = 0;
                                proc_mode = 0;
                            }
                            break;/*
                        case 1:
                            if (data == 0x7E)       //STX
                            {
                                procBuff[proc_index++] = data;
                                threadMessage += string.Format("[{0:X2}]", data);
                                proc_mode++;
                            }
                            else
                            {
                                proc_index = 0;
                                proc_mode = 0;
                            }
                            break;*/
                        case 2:                     //len
                            procBuff[proc_index++] = data;
                            proc_length = data;
                            proc_length--;
                            threadMessage += string.Format(" [{0:X2}]", data); 
                            proc_mode++;
                            break;
                        case 3:
                            procBuff[proc_index++] = data;
                            threadMessage += string.Format(" [{0:X2}]", data);
                            proc_length--;
                            if (proc_length == 0)
                            {
                                proc_mode++;
                            }
                            break;
                        case 4:
                            if (data == 0x7F)
                            {
                                procBuff[proc_index++] = data;
                                threadMessage += string.Format(" [{0:X2}]", data);

                                recvMessage = threadMessage;

                                DataCheck(procBuff);

                                proc_mode = 0;
                                proc_index = 0;
                            }
                            else
                            {
                                proc_mode = 0;
                                proc_index = 0;
                            }
                            break;
                    }
                }
            }
        }


        private void DataCheck(byte[] data)
        {
            /*if(!LogTimer.Enabled)
            {
                btnLogStart.PerformClick();
            }*/
            DateTime newDate = DateTime.Now;
            // recvMessage = Encoding.Default.GetString(data);

            try
            {
                string sensorType = ""; // Master or Slave 표시용

                if (data[3] == 0x01)
                {
                    sensorType = "MASTER";
                    _SensorValue.RecvTime = DateTime.Now;
                    OnVehicleEvent(true, _SensorValue.RecvTime);
                    _SensorValue.x = (BitConverter.ToInt16(new byte[2] { data[5], data[4] }, 0));
                    _SensorValue.y = (BitConverter.ToInt16(new byte[2] { data[7], data[6] }, 0));
                    _SensorValue.z = (BitConverter.ToInt16(new byte[2] { data[9], data[8] }, 0));
                    _SensorValue.rel = (BitConverter.ToInt16(new byte[2] { data[11], data[10] }, 0));
                    _SensorValue.abs_rel = (BitConverter.ToInt16(new byte[2] { data[13], data[12] }, 0));
                    _SensorValue.threshold_T = (BitConverter.ToInt16(new byte[2] { data[15], data[14] }, 0));
                    _SensorValue.state = data[16];
                    _SensorValue.heading = Math.Atan2(_SensorValue.y, _SensorValue.x) * (180 / Math.PI);
                    _SensorValue.traffic_60sec = (BitConverter.ToUInt16(new byte[2] { data[18], data[17] }, 0));
                    _SensorValue.speed = data[19];
                    if (_SensorValue.heading < 0) _SensorValue.heading += 360;
                    SetLable(lbMagnetic_x, String.Format("{0:#,0}", _SensorValue.x));
                    SetLable(lbMagnetic_y, String.Format("{0:#,0}", _SensorValue.y));
                    SetLable(lbMagnetic_z, String.Format("{0:#,0}", _SensorValue.z));
                    SetLable(lbMagnetic_azimuth, String.Format("{0:#,0.0}", _SensorValue.heading));
                    SetLable(lbMagnetic_traffic60sec, String.Format("{0:#,0}", _SensorValue.traffic_60sec));
                    SetLable(lbMagnetic_speed, String.Format("{0:#,0}", _SensorValue.speed));
                    SetLable(lbMagnetic_rel, String.Format("{0:#,0}", _SensorValue.rel));
                    SetLable(lbMagnetic_absrel, String.Format("{0:#,0}", _SensorValue.abs_rel));
                    SetLable(lbMagnetic_threshold_T, String.Format("{0:#,0}", _SensorValue.threshold_T));
                    SetLable(lbMagnetic_state, String.Format("{0:#,0}", _SensorValue.state));
                    currentHeading = (float)_SensorValue.heading;
                }
                else if (data[3] == 0x02)
                {
                    sensorType = "SLAVE";
                    _SensorValue_Slave.RecvTime = DateTime.Now;
                    OnVehicleEvent(false, _SensorValue_Slave.RecvTime);
                    _SensorValue_Slave.x = (BitConverter.ToInt16(new byte[2] { data[5], data[4] }, 0));
                    _SensorValue_Slave.y = (BitConverter.ToInt16(new byte[2] { data[7], data[6] }, 0));
                    _SensorValue_Slave.z = (BitConverter.ToInt16(new byte[2] { data[9], data[8] }, 0));
                    _SensorValue_Slave.rel = (BitConverter.ToInt16(new byte[2] { data[11], data[10] }, 0));
                    _SensorValue_Slave.abs_rel = (BitConverter.ToInt16(new byte[2] { data[13], data[12] }, 0));
                    _SensorValue_Slave.threshold_T = (BitConverter.ToInt16(new byte[2] { data[15], data[14] }, 0));
                    _SensorValue_Slave.state = data[16];
                    _SensorValue_Slave.heading = Math.Atan2(_SensorValue_Slave.y, _SensorValue_Slave.x) * (180 / Math.PI);
                    _SensorValue_Slave.traffic_60sec = (BitConverter.ToUInt16(new byte[2] { data[18], data[17] }, 0));
                    _SensorValue_Slave.speed = data[19];
                    if (_SensorValue_Slave.heading < 0) _SensorValue_Slave.heading += 360;
                    SetLable(lbMagnetic_x_Slave, String.Format("{0:#,0}", _SensorValue_Slave.x));
                    SetLable(lbMagnetic_y_Slave, String.Format("{0:#,0}", _SensorValue_Slave.y));
                    SetLable(lbMagnetic_z_Slave, String.Format("{0:#,0}", _SensorValue_Slave.z));
                    SetLable(lbMagnetic_azimuth_Slave, String.Format("{0:#,0.0}", _SensorValue_Slave.heading));
                    SetLable(lbMagnetic_traffic60sec_Slave, String.Format("{0:#,0}", _SensorValue_Slave.traffic_60sec));
                    SetLable(lbMagnetic_speed_Slave, String.Format("{0:#,0}", _SensorValue_Slave.speed));
                    SetLable(lbMagnetic_rel_Slave, String.Format("{0:#,0}", _SensorValue_Slave.rel));
                    SetLable(lbMagnetic_absrel_Slave, String.Format("{0:#,0}", _SensorValue_Slave.abs_rel));
                    SetLable(lbMagnetic_threshold_T_Slave, String.Format("{0:#,0}", _SensorValue_Slave.threshold_T));
                    SetLable(lbMagnetic_state_Slave, String.Format("{0:#,0}", _SensorValue_Slave.state));
                    currentHeading_Slave = (float)_SensorValue_Slave.heading;
                }
                File_IO_Save();
                // ✅ 마스터 / 슬레이브 구분 로그 추가
                if (!string.IsNullOrEmpty(sensorType))
                {
                    SetText(tb_RecvMsg,
                        $"[{sensorType}] Packet received at {DateTime.Now:HH:mm:ss.fff}",
                        0);
                }
            }
            catch
            {

            }
        }


        public string GetDataTime()
        {
            DateTime newDate = DateTime.Now;
            return newDate.ToString("yyyy-MM-dd HH:mm:ss") + ":" + newDate.Millisecond.ToString("000");
        }

        public void File_IO_Save()
        {
            DateTime LogTime = DateTime.Now;

            FileStream fs;

            string FileName = "LOG\\" + string.Format("{0:S}.log", LogTime.ToString("yyyy-MM-dd"));

            if (!File.Exists(FileName))
                Create_File();

            fs = new FileStream(FileName, FileMode.OpenOrCreate, FileAccess.Write);

            if (fs != null)
            {
                StreamWriter sw = new StreamWriter(fs, System.Text.Encoding.Default);


                fs.Seek(0, SeekOrigin.End);

                sw.Write(LogTime.ToString("yyyy-MM-dd"));
                sw.Write("\t");
                sw.Write(LogTime.ToString("HH:mm:ss[fff]"));
                sw.Write("\t");
                sw.Write(_SensorValue.x.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.y.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.z.ToString());
                sw.Write("\t");

                sw.Write(_SensorValue.heading.ToString());
                sw.Write("\t");

                sw.Write(_SensorValue.traffic_60sec.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.speed.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.rel.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.abs_rel.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.threshold_T.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue.state.ToString());

                sw.Write("\t");
                sw.Write(_SensorValue_Slave.x.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.y.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.z.ToString());
                sw.Write("\t");

                sw.Write(_SensorValue_Slave.heading.ToString());
                sw.Write("\t");

                sw.Write(_SensorValue_Slave.traffic_60sec.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.speed.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.rel.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.abs_rel.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.threshold_T.ToString());
                sw.Write("\t");
                sw.Write(_SensorValue_Slave.state.ToString());

                sw.Write("\t");
                sw.Write(_SensorValue_Final.traffic_60sec.ToString());
                sw.Write("\t");
                sw.Write((_SensorValue_Final.calc_speed.ToString("0.0")));


                sw.Write("\r\n");
                sw.Flush();
                sw.Close();
                fs.Close();

            }

            //File_XLS_Save();
        }

        private void AppendEventLog(string line)
        {
            try
            {
                var now = DateTime.Now;
                // ⚠️ 데이터 테이블 로그와 섞이면 파싱이 깨질 수 있어 별도 파일 권장
                // 같은 파일에 넣고 싶으면 "_event"를 제거하고 yyyy-MM-dd.log 로 쓰세요.
                string dir = "LOG";
                string path = Path.Combine(dir, now.ToString("yyyy-MM-dd") + "_event.log");

                if (!Directory.Exists(dir)) Directory.CreateDirectory(dir);

                lock (_eventLogLock)
                {
                    using (var sw = new StreamWriter(path, append: true, Encoding.Default))
                    {
                        sw.WriteLine(line);
                    }
                }
            }
            catch
            {
                // 파일 에러는 UI를 막지 않도록 무시
            }
        }
        public void SetText(RichTextBox rtb, string text, int opt)
        {
            DateTime Logtime = DateTime.Now;

            string strmessage;

            if (opt == 1)
                strmessage = String.Format("[{0}] - {1}", string.Format(Logtime.ToString("HH:mm:ss[fff]")), text);
            else
                strmessage = String.Format("[{0}] - {1} \r\n", string.Format(Logtime.ToString("HH:mm:ss[fff]")), text);

            if (rtb.InvokeRequired)
            {
                DSetText d = new DSetText(SetText);
                this.Invoke(d, rtb, text, opt);
            }
            else
            {
                if (rtb.Lines.Length > 1000)
                {
                    rtb.Text = rtb.Text.Remove(0, rtb.Lines[0].Length + 1);
                }

                rtb.AppendText(strmessage);

                rtb.ScrollToCaret();
            }

            AppendEventLog(strmessage);
        }

        public void Create_File()
        {
            DateTime Now;

            Now = DateTime.Now;

            DirectoryInfo dir = new DirectoryInfo("LOG\\");

            FileStream fs;

            if (dir.Exists == false)
            {
                dir.Create();
            }

            string FileName = "LOG\\" + string.Format("{0:S}.log", Now.ToString("yyyy-MM-dd"));

            if (!File.Exists(FileName))
            {
                fs = new FileStream(FileName, FileMode.OpenOrCreate, FileAccess.Write);

                StreamWriter sw = new StreamWriter(fs, System.Text.Encoding.Default);
                fs.Seek(0, SeekOrigin.End);


                sw.Write("Magnetic Sensor Monitoring Program V1.0\r\n");
                sw.Write("회 사 명 : (주)래도\r\n");
                sw.Write("작성일자 : " + Now.ToString("yyyy년 MM월 dd일 HH:mm:ss") + "\r\n");
                sw.Write("\r\n\r\n");

                sw.Write("날짜 \t시간 \tX축 \tY축 \tZ축 \t방위 \t분당교통량(min/count) \t차량속도(km/h) \trel \tabs_rel \tthreshold_t \tstate"+
                    "\tX축 \tY축 \tZ축 \t방위 \t분당교통량(min / count) \t차량속도(km / h) \trel \tabs_rel \tthreshold_t \tstate" +
                    "\t최종 분당교통량(min / count) \t최종 차량속도(km / h)" +
                    "\r\n");

                sw.Flush();
                sw.Close();
                fs.Close();
            }

            SetText(tb_RecvMsg, FileName, 0);
        }
        private void tb_RecvMsg_TextChanged(object sender, EventArgs e)
        {
            if (tb_RecvMsg.Text.Length >= 32000)
            {
                tb_RecvMsg.Text = tb_RecvMsg.Text.Substring(20000);

                tb_RecvMsg.SelectionStart = tb_RecvMsg.Text.Length;
                tb_RecvMsg.ScrollToCaret();
            }
        }

        private void btnPortOpen_Click(object sender, EventArgs e)
        {

            if (cbSerialPort.SelectedIndex < 0)
                return;

            sPort = cbSerialPort.SelectedItem.ToString();
            sBaud = cbSerialBaud.SelectedItem.ToString();

            Road1_SerialPort.PortName = sPort;
            Road1_SerialPort.BaudRate = Convert.ToInt32(sBaud); //19200
            Road1_SerialPort.DataBits = 8;
            Road1_SerialPort.StopBits = StopBits.One;
            Road1_SerialPort.Parity = Parity.None;
            Road1_SerialPort.ReadTimeout = 200;
            Road1_SerialPort.Open(); 

            if (Road1_SerialPort.IsOpen)
            {
                WritePrivateProfileString("SERIAL", cbSerialPort.Name, sPort, m_strPath);
                WritePrivateProfileString("SERIAL", cbSerialBaud.Name, sBaud, m_strPath);

                pbLinkStatus.BackColor = Color.Green;
                btnPortClose.Enabled = true;
                btnPortOpen.Enabled = false;
                SetText(tb_RecvMsg, "[INFO ] Megnetic Sensor Connected OK!", 0);
            }
            else
            {
                pbLinkStatus.BackColor = Color.Red;
                btnPortClose.Enabled = false;
                btnPortOpen.Enabled = true;
                SetText(tb_RecvMsg, "[ERROR] Megnetic Sensor Connected Error...!", 0);
            }

        }

        private void btnPortClose_Click(object sender, EventArgs e)
        {
            // if (Monitoring_SerialPort.IsOpen == true)
            // {
            Road1_SerialPort.Close();
            pbLinkStatus.BackColor = Color.Red;
            btnPortClose.Enabled = false;
            btnPortOpen.Enabled = true;
            SetText(tb_RecvMsg, "[ERROR] Megnetic Sensor Disconnected...!\r\n", 0);
            /*            lbATemp.Text = Convert.ToString(Atemp1);
                        lbOTemp.Text = Convert.ToString(Otemp1);*/
            // }
        }


        private void timer1_Tick_1(object sender, EventArgs e)
        {
            if (pbRxDStatus.BackColor == Color.Orange)
                pbRxDStatus.BackColor = Color.White;
            if (pbTxDStatus.BackColor == Color.Yellow)
                pbTxDStatus.BackColor = Color.White;
        }

        double Byte_To_IEEE(byte[] buff, int Index)
        {
            double ret = 0.0;
            byte[] IEEE_Convert = new byte[4];

            Buffer.BlockCopy(buff, Index, IEEE_Convert, 2, 2);
            Buffer.BlockCopy(buff, Index + 2, IEEE_Convert, 0, 2);

            Array.Reverse(IEEE_Convert);

            ret = BitConverter.ToSingle(IEEE_Convert, 0);
            return ret;
        }



        private void chbRequestCode_CheckedChanged(object sender, EventArgs e)
        {
            /*RequestCode = chbRequestCode.Checked;

            if (RequestCode)
            {
                chbRequestCode.Text = "Req";
                timer1.Start();
            }
            else
            {
                chbRequestCode.Text = "No Req";
                timer1.Stop();
            }*/
        }

        private void btnLogStart_Click(object sender, EventArgs e)
        {
            if (LogTimer.Enabled)
            {
                //btnLogStart.ForeColor = Color.Red;
                LogTimer.Stop();
                SetText(tb_RecvMsg, "[RoadTemp Senser] Log Not Save......!\r\n", 0);
            }
            else
            {
                //btnLogStart.ForeColor = Color.LimeGreen;
                LogTimer.Start();
                SetText(tb_RecvMsg, "[RoadTemp Senser] Log Save......!\r\n", 0);
            }
        }

        private void LogTimer_Tick(object sender, EventArgs e)
        {
            File_IO_Save();
        }

        private void pictureBox1_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            g.Clear(Color.Black);

            int centerX = pictureBox1.Width / 2;
            int centerY = pictureBox1.Height / 2;
            int radius = Math.Min(centerX, centerY) - 10;
            /*int imgSize = 270;
            g.DrawImage(pictureBox1.Image, centerX - imgSize / 2, centerY - imgSize / 2, imgSize, imgSize);*/

            // 🔵 배경: 고정된 나침반 원
            for (int angle = 0; angle < 360; angle += 30)
            {
                double rad = (angle - 90) * Math.PI / 180.0;
                float x1 = centerX + (float)(Math.Cos(rad) * (radius - 10));
                float y1 = centerY + (float)(Math.Sin(rad) * (radius - 10));
                float x2 = centerX + (float)(Math.Cos(rad) * radius);
                float y2 = centerY + (float)(Math.Sin(rad) * radius);
                g.DrawLine(Pens.White, x1, y1, x2, y2);

                string label;
                if (angle == 0) label = "N";
                else if (angle == 90) label = "E";
                else if (angle == 180) label = "S";
                else if (angle == 270) label = "W";
                else label = angle.ToString();

                SizeF size = g.MeasureString(label, this.Font);
                float lx = centerX + (float)(Math.Cos(rad) * (radius - 25)) - size.Width / 2;
                float ly = centerY + (float)(Math.Sin(rad) * (radius - 25)) - size.Height / 2;
                g.DrawString(label, this.Font, Brushes.White, lx, ly);
            }

            // ➕ 가운데 십자선
            g.DrawLine(Pens.Gray, centerX - 10, centerY, centerX + 10, centerY);
            g.DrawLine(Pens.Gray, centerX, centerY - 10, centerX, centerY + 10);

            // 🔺 빨간 삼각형 바늘: heading 값에 따라 회전
            g.TranslateTransform(centerX, centerY);
            g.RotateTransform(currentHeading);  // 바늘만 회전
            PointF[] triangle = new PointF[]
            {
        new PointF(0, -radius + 5),
        new PointF(-6, -radius + 20),
        new PointF(6, -radius + 20)
            };
            g.FillPolygon(Brushes.Red, triangle);
            g.ResetTransform();

        }

        private void pictureBox2_Paint(object sender, PaintEventArgs e)
        {
            Graphics g = e.Graphics;
            g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
            g.Clear(Color.Black);

            int centerX = pictureBox1.Width / 2;
            int centerY = pictureBox1.Height / 2;
            int radius = Math.Min(centerX, centerY) - 10;
            /*int imgSize = 270;
            g.DrawImage(pictureBox1.Image, centerX - imgSize / 2, centerY - imgSize / 2, imgSize, imgSize);*/

            // 🔵 배경: 고정된 나침반 원
            for (int angle = 0; angle < 360; angle += 30)
            {
                double rad = (angle - 90) * Math.PI / 180.0;
                float x1 = centerX + (float)(Math.Cos(rad) * (radius - 10));
                float y1 = centerY + (float)(Math.Sin(rad) * (radius - 10));
                float x2 = centerX + (float)(Math.Cos(rad) * radius);
                float y2 = centerY + (float)(Math.Sin(rad) * radius);
                g.DrawLine(Pens.White, x1, y1, x2, y2);

                string label;
                if (angle == 0) label = "N";
                else if (angle == 90) label = "E";
                else if (angle == 180) label = "S";
                else if (angle == 270) label = "W";
                else label = angle.ToString();

                SizeF size = g.MeasureString(label, this.Font);
                float lx = centerX + (float)(Math.Cos(rad) * (radius - 25)) - size.Width / 2;
                float ly = centerY + (float)(Math.Sin(rad) * (radius - 25)) - size.Height / 2;
                g.DrawString(label, this.Font, Brushes.White, lx, ly);
            }

            // ➕ 가운데 십자선
            g.DrawLine(Pens.Gray, centerX - 10, centerY, centerX + 10, centerY);
            g.DrawLine(Pens.Gray, centerX, centerY - 10, centerX, centerY + 10);

            // 🔺 빨간 삼각형 바늘: heading 값에 따라 회전
            g.TranslateTransform(centerX, centerY);
            g.RotateTransform(currentHeading_Slave);  // 바늘만 회전
            PointF[] triangle = new PointF[]
            {
        new PointF(0, -radius + 5),
        new PointF(-6, -radius + 20),
        new PointF(6, -radius + 20)
            };
            g.FillPolygon(Brushes.Red, triangle);
            g.ResetTransform();
        }

        int Byte_To_Int16(byte[] buff, int Index)
        {
            int ret = 0;
            ret = buff[Index] << 8 | buff[Index + 1];
            return ret;
        }

        bool Byte_To_Bool(byte[] buff, int Index, int bit)
        {
            bool ret = false;
            ret = ((buff[Index] >> (bit - 1)) & 0x01) == 0x01 ? true : false; ;
            return ret;
        }

        private void timer2_Tick(object sender, EventArgs e)
        {
            pictureBox1.Invalidate();
            pictureBox2.Invalidate();
        }

        public void SendCommand(string cmd)
        {
            if (Road1_SerialPort.IsOpen)
            {
                cmd += "\r";

                Road1_SerialPort.Write(cmd);
            }

        }
        private void btBLEAT_Click(object sender, EventArgs e)
        {
            string send_cmd = "AT";
            SendCommand(send_cmd);
            pbTxDStatus.BackColor = Color.Yellow;
        }

        private void btBLESCANStert_Click(object sender, EventArgs e)
        {
            string send_cmd = "AT+STARTSCAN";
            SendCommand(send_cmd);
            pbTxDStatus.BackColor = Color.Yellow;
        }

        private void btBLESCANStop_Click(object sender, EventArgs e)
        {
            string send_cmd = "AT+STOPSCAN";
            SendCommand(send_cmd);
            pbTxDStatus.BackColor = Color.Yellow;
        }

        private void btBLEConnect_Click(object sender, EventArgs e)
        {
            if (tbConnectedAddress.Text != "")
            {
                string send_cmd = "AT+CONMAC=" + tbConnectedAddress.Text.ToUpper();
                SendCommand(send_cmd);
            }
        }

        private void btBLEDisConnect_Click(object sender, EventArgs e)
        {
            string send_cmd = "AT+DISCONNECT";
            SendCommand(send_cmd);
            pbTxDStatus.BackColor = Color.Yellow;
        }

        private void btBLESend_Click(object sender, EventArgs e)
        {
            string send_cmd = tbBLECommand.Text.ToUpper();
            tbBLECommand.Text = "";
            SendCommand(send_cmd);

            pbTxDStatus.BackColor = Color.Yellow;
        }

        private void cbSerialPort_Click(object sender, EventArgs e)
        {
            COM_PORT_Init(cbSerialPort, cbSerialBaud);
        }



        private void tbDistance_TextChanged(object sender, EventArgs e)
        {
            WritePrivateProfileString("SETTING", tbDistance.Name, tbDistance.Text, m_strPath);

            // ✅ 거리 파싱 캐시
            if (!double.TryParse(tbDistance.Text, out _distanceMeters))
                _distanceMeters = 5.0;
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            tbDistance.Text = GetProfileString("SETTING", tbDistance.Name, "5", 255, m_strPath);

            if (!double.TryParse(tbDistance.Text, out _distanceMeters))
                _distanceMeters = 5.0;
        }

        // =============================================================
        // Called every time a vehicle-detection packet is received
        // isMaster = true → from Master sensor, false → from Slave
        // =============================================================
        void OnVehicleEvent(bool isMaster, DateTime tEvent)
        {
            lock (_lock) // protect data structures against concurrent access
            {
                // ① Put the event time into the correct queue
                if (isMaster)
                    _evMaster.Enqueue(tEvent);
                else
                    _evSlave.Enqueue(tEvent);

                // ② Try to match oldest Master and Slave detections
                //    (both must have data to compare)
                while (_evMaster.Count > 0 && _evSlave.Count > 0)
                {
                    // Look at the oldest timestamps without removing them
                    var tM = _evMaster.Peek();
                    var tS = _evSlave.Peek();

                    // Calculate absolute time difference (seconds)
                    var dt = Math.Abs((tM - tS).TotalSeconds);

                    // ③ If they are close enough → same vehicle
                    if (dt <= MATCH_WINDOW_SEC)
                    {
                        // Remove both from queues (we’re done with them)
                        _evMaster.Dequeue();
                        _evSlave.Dequeue();

/*                        // ✅ Get the current distance from textbox
                        double distanceMeters = 5.0; // default
                        try
                        {
                            if (!double.TryParse(tbDistance.Text, out distanceMeters))
                                distanceMeters = 5.0; // fallback if user entered invalid value
                        }
                        catch { distanceMeters = 5.0; }*/

                        // Calculate speed only if within sane range
                        var timeSec = Math.Max(dt, 1e-9);
                        if (timeSec >= MIN_TIME_SEC && timeSec <= MAX_TIME_SEC)
                        {
                            // Speed = distance / time × 3.6 → km/h
                            
                            var kmh = (_distanceMeters / timeSec) * 3.6;

                            // Save speed (with median smoothing)
                            CommitSpeed(kmh);

                            // 방향(옵션): 어느 센서가 먼저인가
                            var dir = (tM <= tS) ? "M→S" : "S→M";

                            // ✅ 디버그 라인
                            SetText(tb_RecvMsg, $"[MATCH] dir={dir} dt={timeSec:0.000}s  dist={_distanceMeters:0.00}m  speed={_lastSpeedKmh:0.0} km/h", 0);
                        }

                        // ④ Record a “matched vehicle” timestamp
                        var now = DateTime.Now;
                        _evMatched.Enqueue(now);

                        // Keep only last 60 seconds of matches
                        while (_evMatched.Count > 0 &&
                               (now - _evMatched.Peek()).TotalSeconds > 60.0)
                            _evMatched.Dequeue();

                        // Count = traffic per minute
                        _lastTrafficPerMin = _evMatched.Count;

                        // ⑤ Update UI labels (if present)
                        _SensorValue_Final.calc_speed = _lastSpeedKmh;//(byte)Math.Max(0, Math.Min(255, (int)Math.Round(_lastSpeedKmh)));
                        _SensorValue_Final.traffic_60sec = (ushort)_lastTrafficPerMin;
                        SetLable(lb_Final_speed, $"{_lastSpeedKmh:#,0.0}");
                        SetLable(lb_Fianl_traffic, $"{_lastTrafficPerMin}");

                        File_IO_Save();
                    }
                    else
                    {
                        // ⑥ If difference too large, drop the older timestamp
                        //    This avoids mismatch buildup
                        if (tM < tS)
                            _evMaster.Dequeue();
                        else
                            _evSlave.Dequeue();
                    }
                }
            }
        }

        // =============================================================
        // Median filter: smooths noisy or jittery speed measurements
        // =============================================================
        void CommitSpeed(double kmh)
        {
            _lastSpeedKmh = kmh;
/*            _recentSpeeds.Enqueue(kmh);

            // Keep only the last N measurements
            while (_recentSpeeds.Count > SPEED_MEDIAN_N)
                _recentSpeeds.Dequeue();

            // Sort and pick the middle value → median
            var arr = _recentSpeeds.ToArray();
            Array.Sort(arr);
            _lastSpeedKmh = arr[arr.Length / 2];*/
        }
    }


}

