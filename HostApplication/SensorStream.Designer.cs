namespace RobotSensorStream
{
    partial class frmSensorStream
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea1 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series1 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title1 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea2 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Series series2 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title2 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea3 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend1 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series3 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series4 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series5 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title3 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea4 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend2 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series6 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series7 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series8 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title4 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.Windows.Forms.DataVisualization.Charting.ChartArea chartArea5 = new System.Windows.Forms.DataVisualization.Charting.ChartArea();
            System.Windows.Forms.DataVisualization.Charting.Legend legend3 = new System.Windows.Forms.DataVisualization.Charting.Legend();
            System.Windows.Forms.DataVisualization.Charting.Series series9 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series10 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Series series11 = new System.Windows.Forms.DataVisualization.Charting.Series();
            System.Windows.Forms.DataVisualization.Charting.Title title5 = new System.Windows.Forms.DataVisualization.Charting.Title();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(frmSensorStream));
            this.chtTemperature = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtPressure = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtAcceleration = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtOrientation = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.chtDirection = new System.Windows.Forms.DataVisualization.Charting.Chart();
            this.txtLog = new System.Windows.Forms.TextBox();
            this.cmbPort = new System.Windows.Forms.ComboBox();
            this.spSerialPort = new System.IO.Ports.SerialPort(this.components);
            this.btnConnect = new System.Windows.Forms.Button();
            this.tmrUpdate = new System.Windows.Forms.Timer(this.components);
            this.llblFourWalledCubicle = new System.Windows.Forms.LinkLabel();
            this.llblLUFALib = new System.Windows.Forms.LinkLabel();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.pbLUFA = new System.Windows.Forms.PictureBox();
            this.btnRefreshPorts = new System.Windows.Forms.Button();
            this.pnlUIPositioner = new System.Windows.Forms.Panel();
            ((System.ComponentModel.ISupportInitialize)(this.chtTemperature)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtPressure)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtAcceleration)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtOrientation)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtDirection)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLUFA)).BeginInit();
            this.pnlUIPositioner.SuspendLayout();
            this.SuspendLayout();
            // 
            // chtTemperature
            // 
            this.chtTemperature.BorderlineColor = System.Drawing.Color.Black;
            this.chtTemperature.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea1.AxisX.Interval = 1D;
            chartArea1.AxisX.LabelStyle.Enabled = false;
            chartArea1.AxisX.Title = "Time";
            chartArea1.AxisY.Maximum = 60D;
            chartArea1.AxisY.Minimum = 0D;
            chartArea1.AxisY.Title = "Temperature (Deg C)";
            chartArea1.Name = "chaTempData";
            this.chtTemperature.ChartAreas.Add(chartArea1);
            this.chtTemperature.Location = new System.Drawing.Point(403, 225);
            this.chtTemperature.Name = "chtTemperature";
            series1.BorderWidth = 3;
            series1.ChartArea = "chaTempData";
            series1.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series1.Name = "Temperature";
            this.chtTemperature.Series.Add(series1);
            this.chtTemperature.Size = new System.Drawing.Size(385, 212);
            this.chtTemperature.TabIndex = 0;
            this.chtTemperature.Text = "Temperature";
            title1.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title1.Name = "chtTemperature";
            title1.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title1.ShadowOffset = 1;
            title1.Text = "Temperature";
            this.chtTemperature.Titles.Add(title1);
            // 
            // chtPressure
            // 
            this.chtPressure.BorderlineColor = System.Drawing.Color.Black;
            this.chtPressure.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea2.AxisX.Interval = 1D;
            chartArea2.AxisX.LabelStyle.Enabled = false;
            chartArea2.AxisX.Title = "Time";
            chartArea2.AxisY.LabelStyle.Enabled = false;
            chartArea2.Name = "chaPressureData";
            this.chtPressure.ChartAreas.Add(chartArea2);
            this.chtPressure.Location = new System.Drawing.Point(12, 225);
            this.chtPressure.Name = "chtPressure";
            series2.BorderWidth = 3;
            series2.ChartArea = "chaPressureData";
            series2.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.SplineArea;
            series2.Name = "Temperature";
            series2.XValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            series2.YValueType = System.Windows.Forms.DataVisualization.Charting.ChartValueType.Int32;
            this.chtPressure.Series.Add(series2);
            this.chtPressure.Size = new System.Drawing.Size(385, 212);
            this.chtPressure.TabIndex = 1;
            this.chtPressure.Text = "Pressure";
            title2.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title2.Name = "chtPressure";
            title2.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title2.ShadowOffset = 1;
            title2.Text = "Pressure";
            this.chtPressure.Titles.Add(title2);
            // 
            // chtAcceleration
            // 
            this.chtAcceleration.BorderlineColor = System.Drawing.Color.Black;
            this.chtAcceleration.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea3.AxisX.Interval = 1D;
            chartArea3.AxisX.LabelStyle.Enabled = false;
            chartArea3.AxisX.Title = "Time";
            chartArea3.AxisY.Maximum = 1.3D;
            chartArea3.AxisY.Minimum = -1.3D;
            chartArea3.AxisY.Title = "Acceleration (g)";
            chartArea3.Name = "chaAccelData";
            this.chtAcceleration.ChartAreas.Add(chartArea3);
            legend1.Name = "Legend1";
            this.chtAcceleration.Legends.Add(legend1);
            this.chtAcceleration.Location = new System.Drawing.Point(12, 7);
            this.chtAcceleration.Name = "chtAcceleration";
            series3.BorderWidth = 3;
            series3.ChartArea = "chaAccelData";
            series3.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series3.Legend = "Legend1";
            series3.Name = "X";
            series4.BorderWidth = 3;
            series4.ChartArea = "chaAccelData";
            series4.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series4.Legend = "Legend1";
            series4.Name = "Y";
            series5.BorderWidth = 3;
            series5.ChartArea = "chaAccelData";
            series5.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series5.Legend = "Legend1";
            series5.Name = "Z";
            this.chtAcceleration.Series.Add(series3);
            this.chtAcceleration.Series.Add(series4);
            this.chtAcceleration.Series.Add(series5);
            this.chtAcceleration.Size = new System.Drawing.Size(385, 212);
            this.chtAcceleration.TabIndex = 2;
            this.chtAcceleration.Text = "Acceleration";
            title3.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title3.Name = "chtAcceleration";
            title3.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title3.ShadowOffset = 1;
            title3.Text = "Acceleration";
            this.chtAcceleration.Titles.Add(title3);
            // 
            // chtOrientation
            // 
            this.chtOrientation.BorderlineColor = System.Drawing.Color.Black;
            this.chtOrientation.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea4.AxisX.Interval = 1D;
            chartArea4.AxisX.LabelStyle.Enabled = false;
            chartArea4.AxisX.Title = "Time";
            chartArea4.AxisY.Maximum = 150D;
            chartArea4.AxisY.Minimum = -150D;
            chartArea4.AxisY.Title = "Angle Change (Deg/Sec)";
            chartArea4.Name = "chaOrientationData";
            this.chtOrientation.ChartAreas.Add(chartArea4);
            legend2.Name = "Legend1";
            this.chtOrientation.Legends.Add(legend2);
            this.chtOrientation.Location = new System.Drawing.Point(403, 7);
            this.chtOrientation.Name = "chtOrientation";
            series6.BorderWidth = 3;
            series6.ChartArea = "chaOrientationData";
            series6.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series6.Legend = "Legend1";
            series6.Name = "X";
            series7.BorderWidth = 3;
            series7.ChartArea = "chaOrientationData";
            series7.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series7.Legend = "Legend1";
            series7.Name = "Y";
            series8.BorderWidth = 3;
            series8.ChartArea = "chaOrientationData";
            series8.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series8.Legend = "Legend1";
            series8.Name = "Z";
            this.chtOrientation.Series.Add(series6);
            this.chtOrientation.Series.Add(series7);
            this.chtOrientation.Series.Add(series8);
            this.chtOrientation.Size = new System.Drawing.Size(385, 212);
            this.chtOrientation.TabIndex = 3;
            this.chtOrientation.Text = "Rotation";
            title4.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title4.Name = "chtOrientation";
            title4.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title4.ShadowOffset = 1;
            title4.Text = "Orientation";
            this.chtOrientation.Titles.Add(title4);
            // 
            // chtDirection
            // 
            this.chtDirection.BorderlineColor = System.Drawing.Color.Black;
            this.chtDirection.BorderlineDashStyle = System.Windows.Forms.DataVisualization.Charting.ChartDashStyle.Solid;
            chartArea5.AxisX.Interval = 1D;
            chartArea5.AxisX.LabelStyle.Enabled = false;
            chartArea5.AxisX.Title = "Time";
            chartArea5.AxisY.Maximum = 150D;
            chartArea5.AxisY.Minimum = -150D;
            chartArea5.AxisY.Title = "Magnetic Flux (uT)";
            chartArea5.Name = "chaDirectionData";
            this.chtDirection.ChartAreas.Add(chartArea5);
            legend3.Name = "Legend1";
            this.chtDirection.Legends.Add(legend3);
            this.chtDirection.Location = new System.Drawing.Point(794, 7);
            this.chtDirection.Name = "chtDirection";
            series9.BorderWidth = 3;
            series9.ChartArea = "chaDirectionData";
            series9.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series9.Legend = "Legend1";
            series9.Name = "X";
            series10.BorderWidth = 3;
            series10.ChartArea = "chaDirectionData";
            series10.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series10.Legend = "Legend1";
            series10.Name = "Y";
            series11.BorderWidth = 3;
            series11.ChartArea = "chaDirectionData";
            series11.ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Spline;
            series11.Legend = "Legend1";
            series11.Name = "Z";
            this.chtDirection.Series.Add(series9);
            this.chtDirection.Series.Add(series10);
            this.chtDirection.Series.Add(series11);
            this.chtDirection.Size = new System.Drawing.Size(385, 212);
            this.chtDirection.TabIndex = 4;
            this.chtDirection.Text = "Direction";
            title5.Font = new System.Drawing.Font("Microsoft Sans Serif", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            title5.Name = "chtDirection";
            title5.ShadowColor = System.Drawing.Color.FromArgb(((int)(((byte)(224)))), ((int)(((byte)(224)))), ((int)(((byte)(224)))));
            title5.ShadowOffset = 1;
            title5.Text = "Direction";
            this.chtDirection.Titles.Add(title5);
            // 
            // txtLog
            // 
            this.txtLog.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
            this.txtLog.Location = new System.Drawing.Point(794, 225);
            this.txtLog.Multiline = true;
            this.txtLog.Name = "txtLog";
            this.txtLog.Size = new System.Drawing.Size(385, 212);
            this.txtLog.TabIndex = 5;
            // 
            // cmbPort
            // 
            this.cmbPort.FormattingEnabled = true;
            this.cmbPort.Location = new System.Drawing.Point(487, 473);
            this.cmbPort.Name = "cmbPort";
            this.cmbPort.Size = new System.Drawing.Size(136, 21);
            this.cmbPort.TabIndex = 6;
            this.cmbPort.SelectedIndexChanged += new System.EventHandler(this.cmbPort_SelectedIndexChanged);
            this.cmbPort.TextChanged += new System.EventHandler(this.cmbPort_TextChanged);
            // 
            // spSerialPort
            // 
            this.spSerialPort.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.spSerialPort_DataReceived);
            // 
            // btnConnect
            // 
            this.btnConnect.Enabled = false;
            this.btnConnect.Location = new System.Drawing.Point(629, 473);
            this.btnConnect.Name = "btnConnect";
            this.btnConnect.Size = new System.Drawing.Size(123, 21);
            this.btnConnect.TabIndex = 7;
            this.btnConnect.Text = "Connect";
            this.btnConnect.UseVisualStyleBackColor = true;
            this.btnConnect.Click += new System.EventHandler(this.btnConnect_Click);
            // 
            // tmrUpdate
            // 
            this.tmrUpdate.Enabled = true;
            this.tmrUpdate.Interval = 5;
            this.tmrUpdate.Tick += new System.EventHandler(this.tmrUpdate_Tick);
            // 
            // llblFourWalledCubicle
            // 
            this.llblFourWalledCubicle.Location = new System.Drawing.Point(12, 497);
            this.llblFourWalledCubicle.Name = "llblFourWalledCubicle";
            this.llblFourWalledCubicle.Size = new System.Drawing.Size(141, 13);
            this.llblFourWalledCubicle.TabIndex = 9;
            this.llblFourWalledCubicle.TabStop = true;
            this.llblFourWalledCubicle.Text = "www.fourwalledcubicle.com";
            this.llblFourWalledCubicle.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.llblFourWalledCubicle.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblFourWalledCubicle_LinkClicked);
            // 
            // llblLUFALib
            // 
            this.llblLUFALib.Location = new System.Drawing.Point(1034, 497);
            this.llblLUFALib.Name = "llblLUFALib";
            this.llblLUFALib.Size = new System.Drawing.Size(144, 13);
            this.llblLUFALib.TabIndex = 10;
            this.llblLUFALib.TabStop = true;
            this.llblLUFALib.Text = "www.lufa-lib.org";
            this.llblLUFALib.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.llblLUFALib.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.llblLUFALib_LinkClicked);
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::RobotSensorStream.Properties.Resources.LUFA_thumb;
            this.pictureBox1.Location = new System.Drawing.Point(1037, 443);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(141, 51);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pictureBox1.TabIndex = 12;
            this.pictureBox1.TabStop = false;
            // 
            // pbLUFA
            // 
            this.pbLUFA.Image = global::RobotSensorStream.Properties.Resources.FourWalledCubicle;
            this.pbLUFA.Location = new System.Drawing.Point(12, 443);
            this.pbLUFA.Name = "pbLUFA";
            this.pbLUFA.Size = new System.Drawing.Size(141, 51);
            this.pbLUFA.SizeMode = System.Windows.Forms.PictureBoxSizeMode.Zoom;
            this.pbLUFA.TabIndex = 11;
            this.pbLUFA.TabStop = false;
            // 
            // btnRefreshPorts
            // 
            this.btnRefreshPorts.Image = global::RobotSensorStream.Properties.Resources.action_refresh_blue;
            this.btnRefreshPorts.Location = new System.Drawing.Point(453, 473);
            this.btnRefreshPorts.Name = "btnRefreshPorts";
            this.btnRefreshPorts.Size = new System.Drawing.Size(28, 21);
            this.btnRefreshPorts.TabIndex = 8;
            this.btnRefreshPorts.UseVisualStyleBackColor = true;
            this.btnRefreshPorts.Click += new System.EventHandler(this.btnRefreshPorts_Click);
            // 
            // pnlUIPositioner
            // 
            this.pnlUIPositioner.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.pnlUIPositioner.Controls.Add(this.pictureBox1);
            this.pnlUIPositioner.Controls.Add(this.pbLUFA);
            this.pnlUIPositioner.Controls.Add(this.llblLUFALib);
            this.pnlUIPositioner.Controls.Add(this.llblFourWalledCubicle);
            this.pnlUIPositioner.Controls.Add(this.btnRefreshPorts);
            this.pnlUIPositioner.Controls.Add(this.btnConnect);
            this.pnlUIPositioner.Controls.Add(this.cmbPort);
            this.pnlUIPositioner.Controls.Add(this.txtLog);
            this.pnlUIPositioner.Controls.Add(this.chtDirection);
            this.pnlUIPositioner.Controls.Add(this.chtOrientation);
            this.pnlUIPositioner.Controls.Add(this.chtAcceleration);
            this.pnlUIPositioner.Controls.Add(this.chtPressure);
            this.pnlUIPositioner.Controls.Add(this.chtTemperature);
            this.pnlUIPositioner.Location = new System.Drawing.Point(0, 5);
            this.pnlUIPositioner.Name = "pnlUIPositioner";
            this.pnlUIPositioner.Size = new System.Drawing.Size(1190, 518);
            this.pnlUIPositioner.TabIndex = 13;
            // 
            // frmSensorStream
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1192, 524);
            this.Controls.Add(this.pnlUIPositioner);
            this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
            this.Name = "frmSensorStream";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Explorer Bot Sensor Streamer";
            this.Load += new System.EventHandler(this.frmSensorStream_Load);
            ((System.ComponentModel.ISupportInitialize)(this.chtTemperature)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtPressure)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtAcceleration)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtOrientation)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.chtDirection)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pbLUFA)).EndInit();
            this.pnlUIPositioner.ResumeLayout(false);
            this.pnlUIPositioner.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.DataVisualization.Charting.Chart chtTemperature;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtPressure;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtAcceleration;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtOrientation;
        private System.Windows.Forms.DataVisualization.Charting.Chart chtDirection;
        private System.Windows.Forms.TextBox txtLog;
        private System.Windows.Forms.ComboBox cmbPort;
        private System.IO.Ports.SerialPort spSerialPort;
        private System.Windows.Forms.Button btnConnect;
        private System.Windows.Forms.Timer tmrUpdate;
        private System.Windows.Forms.Button btnRefreshPorts;
        private System.Windows.Forms.LinkLabel llblFourWalledCubicle;
        private System.Windows.Forms.LinkLabel llblLUFALib;
        private System.Windows.Forms.PictureBox pbLUFA;
        private System.Windows.Forms.PictureBox pictureBox1;
        private System.Windows.Forms.Panel pnlUIPositioner;
    }
}

