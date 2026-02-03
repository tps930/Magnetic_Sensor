namespace Wind_Monitoring_Program
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.btnPortClose = new System.Windows.Forms.Button();
            this.btnPortOpen = new System.Windows.Forms.Button();
            this.cbSerialBaud = new System.Windows.Forms.ComboBox();
            this.cbSerialPort = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.backgroundWorker1 = new System.ComponentModel.BackgroundWorker();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label13 = new System.Windows.Forms.Label();
            this.label5 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.pbRxDStatus = new System.Windows.Forms.PictureBox();
            this.pbTxDStatus = new System.Windows.Forms.PictureBox();
            this.pbLinkStatus = new System.Windows.Forms.PictureBox();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.timer1 = new System.Windows.Forms.Timer(this.components);
            this.label6 = new System.Windows.Forms.Label();
            this.flowLayoutPanel2 = new System.Windows.Forms.FlowLayoutPanel();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.panel6 = new System.Windows.Forms.Panel();
            this.lbMagnetic_state = new System.Windows.Forms.Label();
            this.lb = new System.Windows.Forms.Label();
            this.panel5 = new System.Windows.Forms.Panel();
            this.lbMagnetic_threshold_T = new System.Windows.Forms.Label();
            this.label14 = new System.Windows.Forms.Label();
            this.panel4 = new System.Windows.Forms.Panel();
            this.lbMagnetic_absrel = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.panel3 = new System.Windows.Forms.Panel();
            this.lbMagnetic_rel = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.panel2 = new System.Windows.Forms.Panel();
            this.lbMagnetic_speed = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.lbMagnetic_traffic60sec = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.panel9 = new System.Windows.Forms.Panel();
            this.lbMagnetic_azimuth = new System.Windows.Forms.Label();
            this.lbMagnetic_azimuthHeader = new System.Windows.Forms.Label();
            this.panel10 = new System.Windows.Forms.Panel();
            this.lbMagnetic_z = new System.Windows.Forms.Label();
            this.lbMagnetic_zHeader = new System.Windows.Forms.Label();
            this.panel11 = new System.Windows.Forms.Panel();
            this.lbMagnetic_y = new System.Windows.Forms.Label();
            this.lbMagnetic_yHeader = new System.Windows.Forms.Label();
            this.panel12 = new System.Windows.Forms.Panel();
            this.lbMagnetic_x = new System.Windows.Forms.Label();
            this.lbMagnetic_xHeader = new System.Windows.Forms.Label();
            this.panel30 = new System.Windows.Forms.Panel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.panel32 = new System.Windows.Forms.Panel();
            this.tb_RecvMsg = new System.Windows.Forms.RichTextBox();
            this.panel33 = new System.Windows.Forms.Panel();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.btBLESend = new System.Windows.Forms.Button();
            this.btBLEDisConnect = new System.Windows.Forms.Button();
            this.tbBLECommand = new System.Windows.Forms.TextBox();
            this.tbConnectedAddress = new System.Windows.Forms.TextBox();
            this.btBLESCANStop = new System.Windows.Forms.Button();
            this.btBLESCANStert = new System.Windows.Forms.Button();
            this.btBLEAT = new System.Windows.Forms.Button();
            this.btBLEConnect = new System.Windows.Forms.Button();
            this.LogTimer = new System.Windows.Forms.Timer(this.components);
            this.timer2 = new System.Windows.Forms.Timer(this.components);
            this.flowLayoutPanel1 = new System.Windows.Forms.FlowLayoutPanel();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.panel7 = new System.Windows.Forms.Panel();
            this.lbMagnetic_state_Slave = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.panel8 = new System.Windows.Forms.Panel();
            this.lbMagnetic_threshold_T_Slave = new System.Windows.Forms.Label();
            this.label15 = new System.Windows.Forms.Label();
            this.panel13 = new System.Windows.Forms.Panel();
            this.lbMagnetic_absrel_Slave = new System.Windows.Forms.Label();
            this.label17 = new System.Windows.Forms.Label();
            this.panel14 = new System.Windows.Forms.Panel();
            this.lbMagnetic_rel_Slave = new System.Windows.Forms.Label();
            this.label19 = new System.Windows.Forms.Label();
            this.panel15 = new System.Windows.Forms.Panel();
            this.lbMagnetic_speed_Slave = new System.Windows.Forms.Label();
            this.label21 = new System.Windows.Forms.Label();
            this.panel16 = new System.Windows.Forms.Panel();
            this.lbMagnetic_traffic60sec_Slave = new System.Windows.Forms.Label();
            this.label23 = new System.Windows.Forms.Label();
            this.panel17 = new System.Windows.Forms.Panel();
            this.lbMagnetic_azimuth_Slave = new System.Windows.Forms.Label();
            this.label25 = new System.Windows.Forms.Label();
            this.panel18 = new System.Windows.Forms.Panel();
            this.lbMagnetic_z_Slave = new System.Windows.Forms.Label();
            this.label27 = new System.Windows.Forms.Label();
            this.panel19 = new System.Windows.Forms.Panel();
            this.lbMagnetic_y_Slave = new System.Windows.Forms.Label();
            this.label29 = new System.Windows.Forms.Label();
            this.panel20 = new System.Windows.Forms.Panel();
            this.lbMagnetic_x_Slave = new System.Windows.Forms.Label();
            this.label31 = new System.Windows.Forms.Label();
            this.pictureBox2 = new System.Windows.Forms.PictureBox();
            this.groupBox5 = new System.Windows.Forms.GroupBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tbDistance = new System.Windows.Forms.TextBox();
            this.panel21 = new System.Windows.Forms.Panel();
            this.lb_Final_speed = new System.Windows.Forms.Label();
            this.label16 = new System.Windows.Forms.Label();
            this.panel22 = new System.Windows.Forms.Panel();
            this.lb_Fianl_traffic = new System.Windows.Forms.Label();
            this.label20 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbRxDStatus)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbTxDStatus)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLinkStatus)).BeginInit();
            this.flowLayoutPanel2.SuspendLayout();
            this.groupBox6.SuspendLayout();
            this.panel6.SuspendLayout();
            this.panel5.SuspendLayout();
            this.panel4.SuspendLayout();
            this.panel3.SuspendLayout();
            this.panel2.SuspendLayout();
            this.panel1.SuspendLayout();
            this.panel9.SuspendLayout();
            this.panel10.SuspendLayout();
            this.panel11.SuspendLayout();
            this.panel12.SuspendLayout();
            this.panel30.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.panel32.SuspendLayout();
            this.panel33.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.flowLayoutPanel1.SuspendLayout();
            this.groupBox4.SuspendLayout();
            this.panel7.SuspendLayout();
            this.panel8.SuspendLayout();
            this.panel13.SuspendLayout();
            this.panel14.SuspendLayout();
            this.panel15.SuspendLayout();
            this.panel16.SuspendLayout();
            this.panel17.SuspendLayout();
            this.panel18.SuspendLayout();
            this.panel19.SuspendLayout();
            this.panel20.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).BeginInit();
            this.groupBox5.SuspendLayout();
            this.panel21.SuspendLayout();
            this.panel22.SuspendLayout();
            this.SuspendLayout();
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.btnPortClose);
            this.groupBox1.Controls.Add(this.btnPortOpen);
            this.groupBox1.Controls.Add(this.cbSerialBaud);
            this.groupBox1.Controls.Add(this.cbSerialPort);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.ForeColor = System.Drawing.Color.White;
            this.groupBox1.Location = new System.Drawing.Point(9, 2);
            this.groupBox1.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox1.Size = new System.Drawing.Size(420, 59);
            this.groupBox1.TabIndex = 0;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Magnetic Sensor #1";
            // 
            // btnPortClose
            // 
            this.btnPortClose.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnPortClose.ForeColor = System.Drawing.Color.White;
            this.btnPortClose.Location = new System.Drawing.Point(337, 13);
            this.btnPortClose.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnPortClose.Name = "btnPortClose";
            this.btnPortClose.Size = new System.Drawing.Size(72, 23);
            this.btnPortClose.TabIndex = 3;
            this.btnPortClose.Text = "포트닫기";
            this.btnPortClose.UseVisualStyleBackColor = true;
            this.btnPortClose.Click += new System.EventHandler(this.btnPortClose_Click);
            // 
            // btnPortOpen
            // 
            this.btnPortOpen.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btnPortOpen.ForeColor = System.Drawing.Color.White;
            this.btnPortOpen.Location = new System.Drawing.Point(260, 13);
            this.btnPortOpen.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btnPortOpen.Name = "btnPortOpen";
            this.btnPortOpen.Size = new System.Drawing.Size(72, 23);
            this.btnPortOpen.TabIndex = 2;
            this.btnPortOpen.Text = "포트열기";
            this.btnPortOpen.UseVisualStyleBackColor = true;
            this.btnPortOpen.Click += new System.EventHandler(this.btnPortOpen_Click);
            // 
            // cbSerialBaud
            // 
            this.cbSerialBaud.FormattingEnabled = true;
            this.cbSerialBaud.Items.AddRange(new object[] {
            "2400",
            "4800",
            "9600",
            "14400",
            "28800",
            "19200",
            "38400",
            "57600",
            "115200",
            "230400",
            "460800"});
            this.cbSerialBaud.Location = new System.Drawing.Point(162, 17);
            this.cbSerialBaud.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cbSerialBaud.Name = "cbSerialBaud";
            this.cbSerialBaud.Size = new System.Drawing.Size(92, 20);
            this.cbSerialBaud.TabIndex = 2;
            // 
            // cbSerialPort
            // 
            this.cbSerialPort.FormattingEnabled = true;
            this.cbSerialPort.Location = new System.Drawing.Point(66, 17);
            this.cbSerialPort.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.cbSerialPort.Name = "cbSerialPort";
            this.cbSerialPort.Size = new System.Drawing.Size(93, 20);
            this.cbSerialPort.TabIndex = 2;
            this.cbSerialPort.Click += new System.EventHandler(this.cbSerialPort_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.ForeColor = System.Drawing.Color.White;
            this.label2.Location = new System.Drawing.Point(5, 20);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(53, 12);
            this.label2.TabIndex = 2;
            this.label2.Text = "통신포트";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(0, 0);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(38, 12);
            this.label1.TabIndex = 1;
            this.label1.Text = "label1";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label13);
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.pbRxDStatus);
            this.groupBox2.Controls.Add(this.pbTxDStatus);
            this.groupBox2.Controls.Add(this.pbLinkStatus);
            this.groupBox2.ForeColor = System.Drawing.Color.White;
            this.groupBox2.Location = new System.Drawing.Point(437, 2);
            this.groupBox2.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox2.Size = new System.Drawing.Size(269, 59);
            this.groupBox2.TabIndex = 2;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "통신상태";
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.ForeColor = System.Drawing.Color.White;
            this.label13.Location = new System.Drawing.Point(30, 42);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(28, 12);
            this.label13.TabIndex = 7;
            this.label13.Text = "Link";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.ForeColor = System.Drawing.Color.White;
            this.label5.Location = new System.Drawing.Point(207, 42);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(28, 12);
            this.label5.TabIndex = 6;
            this.label5.Text = "RxD";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.ForeColor = System.Drawing.Color.White;
            this.label4.Location = new System.Drawing.Point(119, 42);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(28, 12);
            this.label4.TabIndex = 4;
            this.label4.Text = "TxD";
            // 
            // pbRxDStatus
            // 
            this.pbRxDStatus.BackColor = System.Drawing.Color.White;
            this.pbRxDStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pbRxDStatus.Location = new System.Drawing.Point(186, 16);
            this.pbRxDStatus.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.pbRxDStatus.Name = "pbRxDStatus";
            this.pbRxDStatus.Size = new System.Drawing.Size(72, 20);
            this.pbRxDStatus.TabIndex = 5;
            this.pbRxDStatus.TabStop = false;
            // 
            // pbTxDStatus
            // 
            this.pbTxDStatus.BackColor = System.Drawing.Color.White;
            this.pbTxDStatus.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.pbTxDStatus.Location = new System.Drawing.Point(99, 16);
            this.pbTxDStatus.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.pbTxDStatus.Name = "pbTxDStatus";
            this.pbTxDStatus.Size = new System.Drawing.Size(72, 20);
            this.pbTxDStatus.TabIndex = 4;
            this.pbTxDStatus.TabStop = false;
            // 
            // pbLinkStatus
            // 
            this.pbLinkStatus.BackColor = System.Drawing.Color.White;
            this.pbLinkStatus.Location = new System.Drawing.Point(10, 15);
            this.pbLinkStatus.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.pbLinkStatus.Name = "pbLinkStatus";
            this.pbLinkStatus.Size = new System.Drawing.Size(72, 21);
            this.pbLinkStatus.TabIndex = 3;
            this.pbLinkStatus.TabStop = false;
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.ImageScalingSize = new System.Drawing.Size(20, 20);
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(61, 4);
            // 
            // timer1
            // 
            this.timer1.Enabled = true;
            this.timer1.Interval = 300;
            this.timer1.Tick += new System.EventHandler(this.timer1_Tick_1);
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(0, 0);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(38, 12);
            this.label6.TabIndex = 17;
            this.label6.Text = "label6";
            // 
            // flowLayoutPanel2
            // 
            this.flowLayoutPanel2.Controls.Add(this.groupBox6);
            this.flowLayoutPanel2.Dock = System.Windows.Forms.DockStyle.Left;
            this.flowLayoutPanel2.Location = new System.Drawing.Point(0, 0);
            this.flowLayoutPanel2.Name = "flowLayoutPanel2";
            this.flowLayoutPanel2.Size = new System.Drawing.Size(270, 535);
            this.flowLayoutPanel2.TabIndex = 20;
            // 
            // groupBox6
            // 
            this.groupBox6.Controls.Add(this.panel6);
            this.groupBox6.Controls.Add(this.panel5);
            this.groupBox6.Controls.Add(this.panel4);
            this.groupBox6.Controls.Add(this.panel3);
            this.groupBox6.Controls.Add(this.panel2);
            this.groupBox6.Controls.Add(this.panel1);
            this.groupBox6.Controls.Add(this.panel9);
            this.groupBox6.Controls.Add(this.panel10);
            this.groupBox6.Controls.Add(this.panel11);
            this.groupBox6.Controls.Add(this.panel12);
            this.groupBox6.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox6.ForeColor = System.Drawing.Color.White;
            this.groupBox6.Location = new System.Drawing.Point(3, 3);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(260, 280);
            this.groupBox6.TabIndex = 20;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Magnetic(Master)";
            // 
            // panel6
            // 
            this.panel6.Controls.Add(this.lbMagnetic_state);
            this.panel6.Controls.Add(this.lb);
            this.panel6.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel6.Location = new System.Drawing.Point(3, 246);
            this.panel6.Name = "panel6";
            this.panel6.Size = new System.Drawing.Size(254, 26);
            this.panel6.TabIndex = 9;
            // 
            // lbMagnetic_state
            // 
            this.lbMagnetic_state.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_state.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_state.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_state.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_state.Name = "lbMagnetic_state";
            this.lbMagnetic_state.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_state.TabIndex = 22;
            this.lbMagnetic_state.Text = "0000";
            this.lbMagnetic_state.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lb
            // 
            this.lb.BackColor = System.Drawing.Color.Teal;
            this.lb.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lb.Dock = System.Windows.Forms.DockStyle.Left;
            this.lb.Location = new System.Drawing.Point(0, 0);
            this.lb.Name = "lb";
            this.lb.Size = new System.Drawing.Size(127, 26);
            this.lb.TabIndex = 21;
            this.lb.Text = "state";
            this.lb.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel5
            // 
            this.panel5.Controls.Add(this.lbMagnetic_threshold_T);
            this.panel5.Controls.Add(this.label14);
            this.panel5.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel5.Location = new System.Drawing.Point(3, 220);
            this.panel5.Name = "panel5";
            this.panel5.Size = new System.Drawing.Size(254, 26);
            this.panel5.TabIndex = 8;
            // 
            // lbMagnetic_threshold_T
            // 
            this.lbMagnetic_threshold_T.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_threshold_T.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_threshold_T.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_threshold_T.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_threshold_T.Name = "lbMagnetic_threshold_T";
            this.lbMagnetic_threshold_T.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_threshold_T.TabIndex = 22;
            this.lbMagnetic_threshold_T.Text = "0000";
            this.lbMagnetic_threshold_T.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label14
            // 
            this.label14.BackColor = System.Drawing.Color.Teal;
            this.label14.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label14.Dock = System.Windows.Forms.DockStyle.Left;
            this.label14.Location = new System.Drawing.Point(0, 0);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(127, 26);
            this.label14.TabIndex = 21;
            this.label14.Text = "threshold_T";
            this.label14.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel4
            // 
            this.panel4.Controls.Add(this.lbMagnetic_absrel);
            this.panel4.Controls.Add(this.label11);
            this.panel4.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel4.Location = new System.Drawing.Point(3, 194);
            this.panel4.Name = "panel4";
            this.panel4.Size = new System.Drawing.Size(254, 26);
            this.panel4.TabIndex = 7;
            // 
            // lbMagnetic_absrel
            // 
            this.lbMagnetic_absrel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_absrel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_absrel.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_absrel.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_absrel.Name = "lbMagnetic_absrel";
            this.lbMagnetic_absrel.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_absrel.TabIndex = 22;
            this.lbMagnetic_absrel.Text = "0000";
            this.lbMagnetic_absrel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label11
            // 
            this.label11.BackColor = System.Drawing.Color.Teal;
            this.label11.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label11.Dock = System.Windows.Forms.DockStyle.Left;
            this.label11.Location = new System.Drawing.Point(0, 0);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(127, 26);
            this.label11.TabIndex = 21;
            this.label11.Text = "abs rel";
            this.label11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel3
            // 
            this.panel3.Controls.Add(this.lbMagnetic_rel);
            this.panel3.Controls.Add(this.label8);
            this.panel3.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel3.Location = new System.Drawing.Point(3, 168);
            this.panel3.Name = "panel3";
            this.panel3.Size = new System.Drawing.Size(254, 26);
            this.panel3.TabIndex = 6;
            // 
            // lbMagnetic_rel
            // 
            this.lbMagnetic_rel.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_rel.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_rel.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_rel.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_rel.Name = "lbMagnetic_rel";
            this.lbMagnetic_rel.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_rel.TabIndex = 22;
            this.lbMagnetic_rel.Text = "0000";
            this.lbMagnetic_rel.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label8
            // 
            this.label8.BackColor = System.Drawing.Color.Teal;
            this.label8.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label8.Dock = System.Windows.Forms.DockStyle.Left;
            this.label8.Location = new System.Drawing.Point(0, 0);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(127, 26);
            this.label8.TabIndex = 21;
            this.label8.Text = "rel";
            this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel2
            // 
            this.panel2.Controls.Add(this.lbMagnetic_speed);
            this.panel2.Controls.Add(this.label9);
            this.panel2.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel2.Location = new System.Drawing.Point(3, 142);
            this.panel2.Name = "panel2";
            this.panel2.Size = new System.Drawing.Size(254, 26);
            this.panel2.TabIndex = 5;
            // 
            // lbMagnetic_speed
            // 
            this.lbMagnetic_speed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_speed.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_speed.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_speed.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_speed.Name = "lbMagnetic_speed";
            this.lbMagnetic_speed.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_speed.TabIndex = 22;
            this.lbMagnetic_speed.Text = "0000";
            this.lbMagnetic_speed.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label9
            // 
            this.label9.BackColor = System.Drawing.Color.Teal;
            this.label9.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label9.Dock = System.Windows.Forms.DockStyle.Left;
            this.label9.Location = new System.Drawing.Point(0, 0);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(127, 26);
            this.label9.TabIndex = 21;
            this.label9.Text = "차량속도";
            this.label9.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.lbMagnetic_traffic60sec);
            this.panel1.Controls.Add(this.label7);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel1.Location = new System.Drawing.Point(3, 117);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(254, 25);
            this.panel1.TabIndex = 4;
            // 
            // lbMagnetic_traffic60sec
            // 
            this.lbMagnetic_traffic60sec.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_traffic60sec.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_traffic60sec.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_traffic60sec.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_traffic60sec.Name = "lbMagnetic_traffic60sec";
            this.lbMagnetic_traffic60sec.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_traffic60sec.TabIndex = 22;
            this.lbMagnetic_traffic60sec.Text = "0000";
            this.lbMagnetic_traffic60sec.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label7
            // 
            this.label7.BackColor = System.Drawing.Color.Teal;
            this.label7.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label7.Dock = System.Windows.Forms.DockStyle.Left;
            this.label7.Location = new System.Drawing.Point(0, 0);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(127, 25);
            this.label7.TabIndex = 21;
            this.label7.Text = "교통량";
            this.label7.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel9
            // 
            this.panel9.Controls.Add(this.lbMagnetic_azimuth);
            this.panel9.Controls.Add(this.lbMagnetic_azimuthHeader);
            this.panel9.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel9.Location = new System.Drawing.Point(3, 92);
            this.panel9.Name = "panel9";
            this.panel9.Size = new System.Drawing.Size(254, 25);
            this.panel9.TabIndex = 3;
            // 
            // lbMagnetic_azimuth
            // 
            this.lbMagnetic_azimuth.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_azimuth.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_azimuth.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_azimuth.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_azimuth.Name = "lbMagnetic_azimuth";
            this.lbMagnetic_azimuth.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_azimuth.TabIndex = 22;
            this.lbMagnetic_azimuth.Text = "0000";
            this.lbMagnetic_azimuth.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbMagnetic_azimuthHeader
            // 
            this.lbMagnetic_azimuthHeader.BackColor = System.Drawing.Color.Teal;
            this.lbMagnetic_azimuthHeader.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_azimuthHeader.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_azimuthHeader.Location = new System.Drawing.Point(0, 0);
            this.lbMagnetic_azimuthHeader.Name = "lbMagnetic_azimuthHeader";
            this.lbMagnetic_azimuthHeader.Size = new System.Drawing.Size(127, 25);
            this.lbMagnetic_azimuthHeader.TabIndex = 21;
            this.lbMagnetic_azimuthHeader.Text = "방위각";
            this.lbMagnetic_azimuthHeader.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel10
            // 
            this.panel10.Controls.Add(this.lbMagnetic_z);
            this.panel10.Controls.Add(this.lbMagnetic_zHeader);
            this.panel10.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel10.Location = new System.Drawing.Point(3, 67);
            this.panel10.Name = "panel10";
            this.panel10.Size = new System.Drawing.Size(254, 25);
            this.panel10.TabIndex = 2;
            // 
            // lbMagnetic_z
            // 
            this.lbMagnetic_z.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_z.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_z.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_z.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_z.Name = "lbMagnetic_z";
            this.lbMagnetic_z.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_z.TabIndex = 22;
            this.lbMagnetic_z.Text = "0.0";
            this.lbMagnetic_z.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbMagnetic_zHeader
            // 
            this.lbMagnetic_zHeader.BackColor = System.Drawing.Color.Teal;
            this.lbMagnetic_zHeader.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_zHeader.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_zHeader.Location = new System.Drawing.Point(0, 0);
            this.lbMagnetic_zHeader.Name = "lbMagnetic_zHeader";
            this.lbMagnetic_zHeader.Size = new System.Drawing.Size(127, 25);
            this.lbMagnetic_zHeader.TabIndex = 21;
            this.lbMagnetic_zHeader.Text = "Z";
            this.lbMagnetic_zHeader.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel11
            // 
            this.panel11.Controls.Add(this.lbMagnetic_y);
            this.panel11.Controls.Add(this.lbMagnetic_yHeader);
            this.panel11.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel11.Location = new System.Drawing.Point(3, 42);
            this.panel11.Name = "panel11";
            this.panel11.Size = new System.Drawing.Size(254, 25);
            this.panel11.TabIndex = 1;
            // 
            // lbMagnetic_y
            // 
            this.lbMagnetic_y.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_y.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_y.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_y.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_y.Name = "lbMagnetic_y";
            this.lbMagnetic_y.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_y.TabIndex = 22;
            this.lbMagnetic_y.Text = "0.0";
            this.lbMagnetic_y.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbMagnetic_yHeader
            // 
            this.lbMagnetic_yHeader.BackColor = System.Drawing.Color.Teal;
            this.lbMagnetic_yHeader.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_yHeader.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_yHeader.Location = new System.Drawing.Point(0, 0);
            this.lbMagnetic_yHeader.Name = "lbMagnetic_yHeader";
            this.lbMagnetic_yHeader.Size = new System.Drawing.Size(127, 25);
            this.lbMagnetic_yHeader.TabIndex = 21;
            this.lbMagnetic_yHeader.Text = "Y";
            this.lbMagnetic_yHeader.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel12
            // 
            this.panel12.Controls.Add(this.lbMagnetic_x);
            this.panel12.Controls.Add(this.lbMagnetic_xHeader);
            this.panel12.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel12.Location = new System.Drawing.Point(3, 17);
            this.panel12.Name = "panel12";
            this.panel12.Size = new System.Drawing.Size(254, 25);
            this.panel12.TabIndex = 0;
            // 
            // lbMagnetic_x
            // 
            this.lbMagnetic_x.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_x.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_x.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_x.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_x.Name = "lbMagnetic_x";
            this.lbMagnetic_x.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_x.TabIndex = 22;
            this.lbMagnetic_x.Text = "0000";
            this.lbMagnetic_x.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // lbMagnetic_xHeader
            // 
            this.lbMagnetic_xHeader.BackColor = System.Drawing.Color.Teal;
            this.lbMagnetic_xHeader.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_xHeader.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_xHeader.Location = new System.Drawing.Point(0, 0);
            this.lbMagnetic_xHeader.Name = "lbMagnetic_xHeader";
            this.lbMagnetic_xHeader.Size = new System.Drawing.Size(127, 25);
            this.lbMagnetic_xHeader.TabIndex = 21;
            this.lbMagnetic_xHeader.Text = "X";
            this.lbMagnetic_xHeader.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel30
            // 
            this.panel30.Controls.Add(this.pictureBox2);
            this.panel30.Controls.Add(this.flowLayoutPanel1);
            this.panel30.Controls.Add(this.pictureBox1);
            this.panel30.Controls.Add(this.flowLayoutPanel2);
            this.panel30.Dock = System.Windows.Forms.DockStyle.Left;
            this.panel30.Location = new System.Drawing.Point(0, 0);
            this.panel30.Name = "panel30";
            this.panel30.Size = new System.Drawing.Size(539, 535);
            this.panel30.TabIndex = 29;
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox1.Image")));
            this.pictureBox1.Location = new System.Drawing.Point(0, 289);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(259, 241);
            this.pictureBox1.TabIndex = 0;
            this.pictureBox1.TabStop = false;
            this.pictureBox1.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox1_Paint);
            // 
            // panel32
            // 
            this.panel32.Controls.Add(this.tb_RecvMsg);
            this.panel32.Controls.Add(this.panel30);
            this.panel32.Location = new System.Drawing.Point(46, 219);
            this.panel32.Name = "panel32";
            this.panel32.Size = new System.Drawing.Size(997, 535);
            this.panel32.TabIndex = 31;
            // 
            // tb_RecvMsg
            // 
            this.tb_RecvMsg.CausesValidation = false;
            this.tb_RecvMsg.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tb_RecvMsg.Location = new System.Drawing.Point(539, 0);
            this.tb_RecvMsg.Name = "tb_RecvMsg";
            this.tb_RecvMsg.Size = new System.Drawing.Size(458, 535);
            this.tb_RecvMsg.TabIndex = 31;
            this.tb_RecvMsg.Text = "";
            // 
            // panel33
            // 
            this.panel33.Controls.Add(this.groupBox5);
            this.panel33.Controls.Add(this.groupBox3);
            this.panel33.Controls.Add(this.groupBox1);
            this.panel33.Controls.Add(this.groupBox2);
            this.panel33.Location = new System.Drawing.Point(46, 63);
            this.panel33.Name = "panel33";
            this.panel33.Size = new System.Drawing.Size(997, 149);
            this.panel33.TabIndex = 32;
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.btBLESend);
            this.groupBox3.Controls.Add(this.btBLEDisConnect);
            this.groupBox3.Controls.Add(this.tbBLECommand);
            this.groupBox3.Controls.Add(this.tbConnectedAddress);
            this.groupBox3.Controls.Add(this.btBLESCANStop);
            this.groupBox3.Controls.Add(this.btBLESCANStert);
            this.groupBox3.Controls.Add(this.btBLEAT);
            this.groupBox3.Controls.Add(this.btBLEConnect);
            this.groupBox3.ForeColor = System.Drawing.Color.White;
            this.groupBox3.Location = new System.Drawing.Point(9, 65);
            this.groupBox3.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox3.Size = new System.Drawing.Size(430, 75);
            this.groupBox3.TabIndex = 3;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "BLE Comand";
            // 
            // btBLESend
            // 
            this.btBLESend.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLESend.ForeColor = System.Drawing.Color.White;
            this.btBLESend.Location = new System.Drawing.Point(337, 45);
            this.btBLESend.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLESend.Name = "btBLESend";
            this.btBLESend.Size = new System.Drawing.Size(83, 21);
            this.btBLESend.TabIndex = 10;
            this.btBLESend.Text = "Send";
            this.btBLESend.UseVisualStyleBackColor = true;
            this.btBLESend.Click += new System.EventHandler(this.btBLESend_Click);
            // 
            // btBLEDisConnect
            // 
            this.btBLEDisConnect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLEDisConnect.ForeColor = System.Drawing.Color.White;
            this.btBLEDisConnect.Location = new System.Drawing.Point(337, 18);
            this.btBLEDisConnect.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLEDisConnect.Name = "btBLEDisConnect";
            this.btBLEDisConnect.Size = new System.Drawing.Size(83, 21);
            this.btBLEDisConnect.TabIndex = 9;
            this.btBLEDisConnect.Text = "DisConnect";
            this.btBLEDisConnect.UseVisualStyleBackColor = true;
            this.btBLEDisConnect.Click += new System.EventHandler(this.btBLEDisConnect_Click);
            // 
            // tbBLECommand
            // 
            this.tbBLECommand.Location = new System.Drawing.Point(149, 45);
            this.tbBLECommand.Name = "tbBLECommand";
            this.tbBLECommand.Size = new System.Drawing.Size(183, 21);
            this.tbBLECommand.TabIndex = 8;
            this.tbBLECommand.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // tbConnectedAddress
            // 
            this.tbConnectedAddress.Location = new System.Drawing.Point(149, 18);
            this.tbConnectedAddress.Name = "tbConnectedAddress";
            this.tbConnectedAddress.Size = new System.Drawing.Size(100, 21);
            this.tbConnectedAddress.TabIndex = 7;
            this.tbConnectedAddress.Text = "5CF286468C11";
            this.tbConnectedAddress.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            // 
            // btBLESCANStop
            // 
            this.btBLESCANStop.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLESCANStop.ForeColor = System.Drawing.Color.White;
            this.btBLESCANStop.Location = new System.Drawing.Point(64, 44);
            this.btBLESCANStop.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLESCANStop.Name = "btBLESCANStop";
            this.btBLESCANStop.Size = new System.Drawing.Size(79, 21);
            this.btBLESCANStop.TabIndex = 6;
            this.btBLESCANStop.Text = "Stop SCAN";
            this.btBLESCANStop.UseVisualStyleBackColor = true;
            this.btBLESCANStop.Click += new System.EventHandler(this.btBLESCANStop_Click);
            // 
            // btBLESCANStert
            // 
            this.btBLESCANStert.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLESCANStert.ForeColor = System.Drawing.Color.White;
            this.btBLESCANStert.Location = new System.Drawing.Point(64, 18);
            this.btBLESCANStert.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLESCANStert.Name = "btBLESCANStert";
            this.btBLESCANStert.Size = new System.Drawing.Size(79, 21);
            this.btBLESCANStert.TabIndex = 5;
            this.btBLESCANStert.Text = "Start SCAN";
            this.btBLESCANStert.UseVisualStyleBackColor = true;
            this.btBLESCANStert.Click += new System.EventHandler(this.btBLESCANStert_Click);
            // 
            // btBLEAT
            // 
            this.btBLEAT.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLEAT.ForeColor = System.Drawing.Color.White;
            this.btBLEAT.Location = new System.Drawing.Point(7, 18);
            this.btBLEAT.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLEAT.Name = "btBLEAT";
            this.btBLEAT.Size = new System.Drawing.Size(51, 47);
            this.btBLEAT.TabIndex = 4;
            this.btBLEAT.Text = "AT";
            this.btBLEAT.UseVisualStyleBackColor = true;
            this.btBLEAT.Click += new System.EventHandler(this.btBLEAT_Click);
            // 
            // btBLEConnect
            // 
            this.btBLEConnect.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
            this.btBLEConnect.ForeColor = System.Drawing.Color.White;
            this.btBLEConnect.Location = new System.Drawing.Point(255, 18);
            this.btBLEConnect.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.btBLEConnect.Name = "btBLEConnect";
            this.btBLEConnect.Size = new System.Drawing.Size(77, 21);
            this.btBLEConnect.TabIndex = 2;
            this.btBLEConnect.Text = "Connect";
            this.btBLEConnect.UseVisualStyleBackColor = true;
            this.btBLEConnect.Click += new System.EventHandler(this.btBLEConnect_Click);
            // 
            // LogTimer
            // 
            this.LogTimer.Interval = 1000;
            this.LogTimer.Tick += new System.EventHandler(this.LogTimer_Tick);
            // 
            // timer2
            // 
            this.timer2.Enabled = true;
            this.timer2.Interval = 50;
            this.timer2.Tick += new System.EventHandler(this.timer2_Tick);
            // 
            // flowLayoutPanel1
            // 
            this.flowLayoutPanel1.Controls.Add(this.groupBox4);
            this.flowLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Top;
            this.flowLayoutPanel1.Location = new System.Drawing.Point(270, 0);
            this.flowLayoutPanel1.Name = "flowLayoutPanel1";
            this.flowLayoutPanel1.Size = new System.Drawing.Size(269, 283);
            this.flowLayoutPanel1.TabIndex = 21;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.panel7);
            this.groupBox4.Controls.Add(this.panel8);
            this.groupBox4.Controls.Add(this.panel13);
            this.groupBox4.Controls.Add(this.panel14);
            this.groupBox4.Controls.Add(this.panel15);
            this.groupBox4.Controls.Add(this.panel16);
            this.groupBox4.Controls.Add(this.panel17);
            this.groupBox4.Controls.Add(this.panel18);
            this.groupBox4.Controls.Add(this.panel19);
            this.groupBox4.Controls.Add(this.panel20);
            this.groupBox4.Dock = System.Windows.Forms.DockStyle.Top;
            this.groupBox4.ForeColor = System.Drawing.Color.White;
            this.groupBox4.Location = new System.Drawing.Point(3, 3);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(260, 280);
            this.groupBox4.TabIndex = 20;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Magnetic(Slave)";
            // 
            // panel7
            // 
            this.panel7.Controls.Add(this.lbMagnetic_state_Slave);
            this.panel7.Controls.Add(this.label10);
            this.panel7.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel7.Location = new System.Drawing.Point(3, 246);
            this.panel7.Name = "panel7";
            this.panel7.Size = new System.Drawing.Size(254, 26);
            this.panel7.TabIndex = 9;
            // 
            // lbMagnetic_state_Slave
            // 
            this.lbMagnetic_state_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_state_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_state_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_state_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_state_Slave.Name = "lbMagnetic_state_Slave";
            this.lbMagnetic_state_Slave.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_state_Slave.TabIndex = 22;
            this.lbMagnetic_state_Slave.Text = "0000";
            this.lbMagnetic_state_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label10
            // 
            this.label10.BackColor = System.Drawing.Color.Teal;
            this.label10.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label10.Dock = System.Windows.Forms.DockStyle.Left;
            this.label10.Location = new System.Drawing.Point(0, 0);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(127, 26);
            this.label10.TabIndex = 21;
            this.label10.Text = "state";
            this.label10.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel8
            // 
            this.panel8.Controls.Add(this.lbMagnetic_threshold_T_Slave);
            this.panel8.Controls.Add(this.label15);
            this.panel8.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel8.Location = new System.Drawing.Point(3, 220);
            this.panel8.Name = "panel8";
            this.panel8.Size = new System.Drawing.Size(254, 26);
            this.panel8.TabIndex = 8;
            // 
            // lbMagnetic_threshold_T_Slave
            // 
            this.lbMagnetic_threshold_T_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_threshold_T_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_threshold_T_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_threshold_T_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_threshold_T_Slave.Name = "lbMagnetic_threshold_T_Slave";
            this.lbMagnetic_threshold_T_Slave.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_threshold_T_Slave.TabIndex = 22;
            this.lbMagnetic_threshold_T_Slave.Text = "0000";
            this.lbMagnetic_threshold_T_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label15
            // 
            this.label15.BackColor = System.Drawing.Color.Teal;
            this.label15.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label15.Dock = System.Windows.Forms.DockStyle.Left;
            this.label15.Location = new System.Drawing.Point(0, 0);
            this.label15.Name = "label15";
            this.label15.Size = new System.Drawing.Size(127, 26);
            this.label15.TabIndex = 21;
            this.label15.Text = "threshold_T";
            this.label15.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel13
            // 
            this.panel13.Controls.Add(this.lbMagnetic_absrel_Slave);
            this.panel13.Controls.Add(this.label17);
            this.panel13.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel13.Location = new System.Drawing.Point(3, 194);
            this.panel13.Name = "panel13";
            this.panel13.Size = new System.Drawing.Size(254, 26);
            this.panel13.TabIndex = 7;
            // 
            // lbMagnetic_absrel_Slave
            // 
            this.lbMagnetic_absrel_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_absrel_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_absrel_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_absrel_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_absrel_Slave.Name = "lbMagnetic_absrel_Slave";
            this.lbMagnetic_absrel_Slave.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_absrel_Slave.TabIndex = 22;
            this.lbMagnetic_absrel_Slave.Text = "0000";
            this.lbMagnetic_absrel_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label17
            // 
            this.label17.BackColor = System.Drawing.Color.Teal;
            this.label17.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label17.Dock = System.Windows.Forms.DockStyle.Left;
            this.label17.Location = new System.Drawing.Point(0, 0);
            this.label17.Name = "label17";
            this.label17.Size = new System.Drawing.Size(127, 26);
            this.label17.TabIndex = 21;
            this.label17.Text = "abs rel";
            this.label17.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel14
            // 
            this.panel14.Controls.Add(this.lbMagnetic_rel_Slave);
            this.panel14.Controls.Add(this.label19);
            this.panel14.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel14.Location = new System.Drawing.Point(3, 168);
            this.panel14.Name = "panel14";
            this.panel14.Size = new System.Drawing.Size(254, 26);
            this.panel14.TabIndex = 6;
            // 
            // lbMagnetic_rel_Slave
            // 
            this.lbMagnetic_rel_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_rel_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_rel_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_rel_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_rel_Slave.Name = "lbMagnetic_rel_Slave";
            this.lbMagnetic_rel_Slave.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_rel_Slave.TabIndex = 22;
            this.lbMagnetic_rel_Slave.Text = "0000";
            this.lbMagnetic_rel_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label19
            // 
            this.label19.BackColor = System.Drawing.Color.Teal;
            this.label19.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label19.Dock = System.Windows.Forms.DockStyle.Left;
            this.label19.Location = new System.Drawing.Point(0, 0);
            this.label19.Name = "label19";
            this.label19.Size = new System.Drawing.Size(127, 26);
            this.label19.TabIndex = 21;
            this.label19.Text = "rel";
            this.label19.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel15
            // 
            this.panel15.Controls.Add(this.lbMagnetic_speed_Slave);
            this.panel15.Controls.Add(this.label21);
            this.panel15.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel15.Location = new System.Drawing.Point(3, 142);
            this.panel15.Name = "panel15";
            this.panel15.Size = new System.Drawing.Size(254, 26);
            this.panel15.TabIndex = 5;
            // 
            // lbMagnetic_speed_Slave
            // 
            this.lbMagnetic_speed_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_speed_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_speed_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_speed_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_speed_Slave.Name = "lbMagnetic_speed_Slave";
            this.lbMagnetic_speed_Slave.Size = new System.Drawing.Size(126, 26);
            this.lbMagnetic_speed_Slave.TabIndex = 22;
            this.lbMagnetic_speed_Slave.Text = "0000";
            this.lbMagnetic_speed_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label21
            // 
            this.label21.BackColor = System.Drawing.Color.Teal;
            this.label21.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label21.Dock = System.Windows.Forms.DockStyle.Left;
            this.label21.Location = new System.Drawing.Point(0, 0);
            this.label21.Name = "label21";
            this.label21.Size = new System.Drawing.Size(127, 26);
            this.label21.TabIndex = 21;
            this.label21.Text = "차량속도";
            this.label21.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel16
            // 
            this.panel16.Controls.Add(this.lbMagnetic_traffic60sec_Slave);
            this.panel16.Controls.Add(this.label23);
            this.panel16.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel16.Location = new System.Drawing.Point(3, 117);
            this.panel16.Name = "panel16";
            this.panel16.Size = new System.Drawing.Size(254, 25);
            this.panel16.TabIndex = 4;
            // 
            // lbMagnetic_traffic60sec_Slave
            // 
            this.lbMagnetic_traffic60sec_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_traffic60sec_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_traffic60sec_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_traffic60sec_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_traffic60sec_Slave.Name = "lbMagnetic_traffic60sec_Slave";
            this.lbMagnetic_traffic60sec_Slave.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_traffic60sec_Slave.TabIndex = 22;
            this.lbMagnetic_traffic60sec_Slave.Text = "0000";
            this.lbMagnetic_traffic60sec_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label23
            // 
            this.label23.BackColor = System.Drawing.Color.Teal;
            this.label23.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label23.Dock = System.Windows.Forms.DockStyle.Left;
            this.label23.Location = new System.Drawing.Point(0, 0);
            this.label23.Name = "label23";
            this.label23.Size = new System.Drawing.Size(127, 25);
            this.label23.TabIndex = 21;
            this.label23.Text = "교통량";
            this.label23.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel17
            // 
            this.panel17.Controls.Add(this.lbMagnetic_azimuth_Slave);
            this.panel17.Controls.Add(this.label25);
            this.panel17.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel17.Location = new System.Drawing.Point(3, 92);
            this.panel17.Name = "panel17";
            this.panel17.Size = new System.Drawing.Size(254, 25);
            this.panel17.TabIndex = 3;
            // 
            // lbMagnetic_azimuth_Slave
            // 
            this.lbMagnetic_azimuth_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_azimuth_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_azimuth_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_azimuth_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_azimuth_Slave.Name = "lbMagnetic_azimuth_Slave";
            this.lbMagnetic_azimuth_Slave.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_azimuth_Slave.TabIndex = 22;
            this.lbMagnetic_azimuth_Slave.Text = "0000";
            this.lbMagnetic_azimuth_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label25
            // 
            this.label25.BackColor = System.Drawing.Color.Teal;
            this.label25.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label25.Dock = System.Windows.Forms.DockStyle.Left;
            this.label25.Location = new System.Drawing.Point(0, 0);
            this.label25.Name = "label25";
            this.label25.Size = new System.Drawing.Size(127, 25);
            this.label25.TabIndex = 21;
            this.label25.Text = "방위각";
            this.label25.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel18
            // 
            this.panel18.Controls.Add(this.lbMagnetic_z_Slave);
            this.panel18.Controls.Add(this.label27);
            this.panel18.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel18.Location = new System.Drawing.Point(3, 67);
            this.panel18.Name = "panel18";
            this.panel18.Size = new System.Drawing.Size(254, 25);
            this.panel18.TabIndex = 2;
            // 
            // lbMagnetic_z_Slave
            // 
            this.lbMagnetic_z_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_z_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_z_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_z_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_z_Slave.Name = "lbMagnetic_z_Slave";
            this.lbMagnetic_z_Slave.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_z_Slave.TabIndex = 22;
            this.lbMagnetic_z_Slave.Text = "0.0";
            this.lbMagnetic_z_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label27
            // 
            this.label27.BackColor = System.Drawing.Color.Teal;
            this.label27.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label27.Dock = System.Windows.Forms.DockStyle.Left;
            this.label27.Location = new System.Drawing.Point(0, 0);
            this.label27.Name = "label27";
            this.label27.Size = new System.Drawing.Size(127, 25);
            this.label27.TabIndex = 21;
            this.label27.Text = "Z";
            this.label27.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel19
            // 
            this.panel19.Controls.Add(this.lbMagnetic_y_Slave);
            this.panel19.Controls.Add(this.label29);
            this.panel19.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel19.Location = new System.Drawing.Point(3, 42);
            this.panel19.Name = "panel19";
            this.panel19.Size = new System.Drawing.Size(254, 25);
            this.panel19.TabIndex = 1;
            // 
            // lbMagnetic_y_Slave
            // 
            this.lbMagnetic_y_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_y_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_y_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_y_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_y_Slave.Name = "lbMagnetic_y_Slave";
            this.lbMagnetic_y_Slave.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_y_Slave.TabIndex = 22;
            this.lbMagnetic_y_Slave.Text = "0.0";
            this.lbMagnetic_y_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label29
            // 
            this.label29.BackColor = System.Drawing.Color.Teal;
            this.label29.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label29.Dock = System.Windows.Forms.DockStyle.Left;
            this.label29.Location = new System.Drawing.Point(0, 0);
            this.label29.Name = "label29";
            this.label29.Size = new System.Drawing.Size(127, 25);
            this.label29.TabIndex = 21;
            this.label29.Text = "Y";
            this.label29.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel20
            // 
            this.panel20.Controls.Add(this.lbMagnetic_x_Slave);
            this.panel20.Controls.Add(this.label31);
            this.panel20.Dock = System.Windows.Forms.DockStyle.Top;
            this.panel20.Location = new System.Drawing.Point(3, 17);
            this.panel20.Name = "panel20";
            this.panel20.Size = new System.Drawing.Size(254, 25);
            this.panel20.TabIndex = 0;
            // 
            // lbMagnetic_x_Slave
            // 
            this.lbMagnetic_x_Slave.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lbMagnetic_x_Slave.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lbMagnetic_x_Slave.Dock = System.Windows.Forms.DockStyle.Left;
            this.lbMagnetic_x_Slave.Location = new System.Drawing.Point(127, 0);
            this.lbMagnetic_x_Slave.Name = "lbMagnetic_x_Slave";
            this.lbMagnetic_x_Slave.Size = new System.Drawing.Size(126, 25);
            this.lbMagnetic_x_Slave.TabIndex = 22;
            this.lbMagnetic_x_Slave.Text = "0000";
            this.lbMagnetic_x_Slave.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label31
            // 
            this.label31.BackColor = System.Drawing.Color.Teal;
            this.label31.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label31.Dock = System.Windows.Forms.DockStyle.Left;
            this.label31.Location = new System.Drawing.Point(0, 0);
            this.label31.Name = "label31";
            this.label31.Size = new System.Drawing.Size(127, 25);
            this.label31.TabIndex = 21;
            this.label31.Text = "X";
            this.label31.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // pictureBox2
            // 
            this.pictureBox2.Image = ((System.Drawing.Image)(resources.GetObject("pictureBox2.Image")));
            this.pictureBox2.Location = new System.Drawing.Point(275, 289);
            this.pictureBox2.Name = "pictureBox2";
            this.pictureBox2.Size = new System.Drawing.Size(259, 241);
            this.pictureBox2.TabIndex = 22;
            this.pictureBox2.TabStop = false;
            this.pictureBox2.Paint += new System.Windows.Forms.PaintEventHandler(this.pictureBox2_Paint);
            // 
            // groupBox5
            // 
            this.groupBox5.Controls.Add(this.panel22);
            this.groupBox5.Controls.Add(this.panel21);
            this.groupBox5.Controls.Add(this.tbDistance);
            this.groupBox5.Controls.Add(this.label3);
            this.groupBox5.ForeColor = System.Drawing.Color.White;
            this.groupBox5.Location = new System.Drawing.Point(447, 72);
            this.groupBox5.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Name = "groupBox5";
            this.groupBox5.Padding = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.groupBox5.Size = new System.Drawing.Size(342, 68);
            this.groupBox5.TabIndex = 4;
            this.groupBox5.TabStop = false;
            this.groupBox5.Text = "최종 결과";
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.ForeColor = System.Drawing.Color.White;
            this.label3.Location = new System.Drawing.Point(4, 17);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(78, 12);
            this.label3.TabIndex = 2;
            this.label3.Text = "센서 간격(m)";
            // 
            // tbDistance
            // 
            this.tbDistance.Location = new System.Drawing.Point(6, 39);
            this.tbDistance.Name = "tbDistance";
            this.tbDistance.Size = new System.Drawing.Size(72, 21);
            this.tbDistance.TabIndex = 9;
            this.tbDistance.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;
            this.tbDistance.TextChanged += new System.EventHandler(this.tbDistance_TextChanged);
            // 
            // panel21
            // 
            this.panel21.Controls.Add(this.lb_Final_speed);
            this.panel21.Controls.Add(this.label16);
            this.panel21.Location = new System.Drawing.Point(83, 38);
            this.panel21.Name = "panel21";
            this.panel21.Size = new System.Drawing.Size(254, 26);
            this.panel21.TabIndex = 10;
            // 
            // lb_Final_speed
            // 
            this.lb_Final_speed.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lb_Final_speed.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lb_Final_speed.Dock = System.Windows.Forms.DockStyle.Left;
            this.lb_Final_speed.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lb_Final_speed.Location = new System.Drawing.Point(127, 0);
            this.lb_Final_speed.Name = "lb_Final_speed";
            this.lb_Final_speed.Size = new System.Drawing.Size(126, 26);
            this.lb_Final_speed.TabIndex = 22;
            this.lb_Final_speed.Text = "0000";
            this.lb_Final_speed.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label16
            // 
            this.label16.BackColor = System.Drawing.Color.Teal;
            this.label16.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label16.Dock = System.Windows.Forms.DockStyle.Left;
            this.label16.Location = new System.Drawing.Point(0, 0);
            this.label16.Name = "label16";
            this.label16.Size = new System.Drawing.Size(127, 26);
            this.label16.TabIndex = 21;
            this.label16.Text = "차량속도";
            this.label16.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // panel22
            // 
            this.panel22.Controls.Add(this.lb_Fianl_traffic);
            this.panel22.Controls.Add(this.label20);
            this.panel22.Location = new System.Drawing.Point(83, 11);
            this.panel22.Name = "panel22";
            this.panel22.Size = new System.Drawing.Size(254, 25);
            this.panel22.TabIndex = 11;
            // 
            // lb_Fianl_traffic
            // 
            this.lb_Fianl_traffic.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(0)))), ((int)(((byte)(64)))), ((int)(((byte)(64)))));
            this.lb_Fianl_traffic.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.lb_Fianl_traffic.Dock = System.Windows.Forms.DockStyle.Left;
            this.lb_Fianl_traffic.Font = new System.Drawing.Font("굴림", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.lb_Fianl_traffic.Location = new System.Drawing.Point(127, 0);
            this.lb_Fianl_traffic.Name = "lb_Fianl_traffic";
            this.lb_Fianl_traffic.Size = new System.Drawing.Size(126, 25);
            this.lb_Fianl_traffic.TabIndex = 22;
            this.lb_Fianl_traffic.Text = "0000";
            this.lb_Fianl_traffic.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // label20
            // 
            this.label20.BackColor = System.Drawing.Color.Teal;
            this.label20.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
            this.label20.Dock = System.Windows.Forms.DockStyle.Left;
            this.label20.Location = new System.Drawing.Point(0, 0);
            this.label20.Name = "label20";
            this.label20.Size = new System.Drawing.Size(127, 25);
            this.label20.TabIndex = 21;
            this.label20.Text = "교통량";
            this.label20.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1079, 767);
            this.Controls.Add(this.panel33);
            this.Controls.Add(this.panel32);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label1);
            this.Margin = new System.Windows.Forms.Padding(3, 2, 3, 2);
            this.Name = "Form1";
            this.Padding = new System.Windows.Forms.Padding(10, 60, 10, 10);
            this.Style = MetroFramework.MetroColorStyle.Red;
            this.Text = "Magnatic Sensor Program v1.0";
            this.Theme = MetroFramework.MetroThemeStyle.Dark;
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.pbRxDStatus)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbTxDStatus)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLinkStatus)).EndInit();
            this.flowLayoutPanel2.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.panel6.ResumeLayout(false);
            this.panel5.ResumeLayout(false);
            this.panel4.ResumeLayout(false);
            this.panel3.ResumeLayout(false);
            this.panel2.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel9.ResumeLayout(false);
            this.panel10.ResumeLayout(false);
            this.panel11.ResumeLayout(false);
            this.panel12.ResumeLayout(false);
            this.panel30.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.panel32.ResumeLayout(false);
            this.panel33.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.flowLayoutPanel1.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.panel7.ResumeLayout(false);
            this.panel8.ResumeLayout(false);
            this.panel13.ResumeLayout(false);
            this.panel14.ResumeLayout(false);
            this.panel15.ResumeLayout(false);
            this.panel16.ResumeLayout(false);
            this.panel17.ResumeLayout(false);
            this.panel18.ResumeLayout(false);
            this.panel19.ResumeLayout(false);
            this.panel20.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox2)).EndInit();
            this.groupBox5.ResumeLayout(false);
            this.groupBox5.PerformLayout();
            this.panel21.ResumeLayout(false);
            this.panel22.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.GroupBox groupBox1;
        private System.ComponentModel.BackgroundWorker backgroundWorker1;
        private System.Windows.Forms.ComboBox cbSerialBaud;
        private System.Windows.Forms.ComboBox cbSerialPort;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        public System.Windows.Forms.Button btnPortOpen;
        public System.Windows.Forms.Button btnPortClose;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.PictureBox pbRxDStatus;
        private System.Windows.Forms.PictureBox pbLinkStatus;
        public System.Windows.Forms.PictureBox pbTxDStatus;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.Timer timer1;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel2;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Panel panel9;
        private System.Windows.Forms.Label lbMagnetic_azimuth;
        private System.Windows.Forms.Label lbMagnetic_azimuthHeader;
        private System.Windows.Forms.Panel panel10;
        private System.Windows.Forms.Label lbMagnetic_z;
        private System.Windows.Forms.Label lbMagnetic_zHeader;
        private System.Windows.Forms.Panel panel11;
        private System.Windows.Forms.Label lbMagnetic_y;
        private System.Windows.Forms.Label lbMagnetic_yHeader;
        private System.Windows.Forms.Panel panel12;
        private System.Windows.Forms.Label lbMagnetic_x;
        private System.Windows.Forms.Label lbMagnetic_xHeader;
        private System.Windows.Forms.Panel panel30;
        private System.Windows.Forms.Panel panel32;
        private System.Windows.Forms.Panel panel33;
        private System.Windows.Forms.Timer LogTimer;
        private System.Windows.Forms.RichTextBox tb_RecvMsg;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Timer timer2;
        private System.Windows.Forms.Panel panel2;
        private System.Windows.Forms.Label lbMagnetic_speed;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label lbMagnetic_traffic60sec;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.GroupBox groupBox3;
        public System.Windows.Forms.Button btBLESend;
        public System.Windows.Forms.Button btBLEDisConnect;
        private System.Windows.Forms.TextBox tbBLECommand;
        private System.Windows.Forms.TextBox tbConnectedAddress;
        public System.Windows.Forms.Button btBLESCANStop;
        public System.Windows.Forms.Button btBLESCANStert;
        public System.Windows.Forms.Button btBLEAT;
        public System.Windows.Forms.Button btBLEConnect;
        private System.Windows.Forms.Panel panel5;
        private System.Windows.Forms.Label lbMagnetic_threshold_T;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.Panel panel4;
        private System.Windows.Forms.Label lbMagnetic_absrel;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.Panel panel3;
        private System.Windows.Forms.Label lbMagnetic_rel;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.Panel panel6;
        private System.Windows.Forms.Label lbMagnetic_state;
        private System.Windows.Forms.Label lb;
        private System.Windows.Forms.PictureBox pictureBox2;
        private System.Windows.Forms.FlowLayoutPanel flowLayoutPanel1;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Panel panel7;
        private System.Windows.Forms.Label lbMagnetic_state_Slave;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.Panel panel8;
        private System.Windows.Forms.Label lbMagnetic_threshold_T_Slave;
        private System.Windows.Forms.Label label15;
        private System.Windows.Forms.Panel panel13;
        private System.Windows.Forms.Label lbMagnetic_absrel_Slave;
        private System.Windows.Forms.Label label17;
        private System.Windows.Forms.Panel panel14;
        private System.Windows.Forms.Label lbMagnetic_rel_Slave;
        private System.Windows.Forms.Label label19;
        private System.Windows.Forms.Panel panel15;
        private System.Windows.Forms.Label lbMagnetic_speed_Slave;
        private System.Windows.Forms.Label label21;
        private System.Windows.Forms.Panel panel16;
        private System.Windows.Forms.Label lbMagnetic_traffic60sec_Slave;
        private System.Windows.Forms.Label label23;
        private System.Windows.Forms.Panel panel17;
        private System.Windows.Forms.Label lbMagnetic_azimuth_Slave;
        private System.Windows.Forms.Label label25;
        private System.Windows.Forms.Panel panel18;
        private System.Windows.Forms.Label lbMagnetic_z_Slave;
        private System.Windows.Forms.Label label27;
        private System.Windows.Forms.Panel panel19;
        private System.Windows.Forms.Label lbMagnetic_y_Slave;
        private System.Windows.Forms.Label label29;
        private System.Windows.Forms.Panel panel20;
        private System.Windows.Forms.Label lbMagnetic_x_Slave;
        private System.Windows.Forms.Label label31;
        private System.Windows.Forms.GroupBox groupBox5;
        private System.Windows.Forms.Panel panel22;
        private System.Windows.Forms.Label lb_Fianl_traffic;
        private System.Windows.Forms.Label label20;
        private System.Windows.Forms.Panel panel21;
        private System.Windows.Forms.Label lb_Final_speed;
        private System.Windows.Forms.Label label16;
        private System.Windows.Forms.TextBox tbDistance;
        private System.Windows.Forms.Label label3;
    }
}

