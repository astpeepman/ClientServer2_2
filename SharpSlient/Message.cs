using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.FileIO;

namespace SharpSlient
{
	
	public enum Messages
	{
		M_CREATE,
		M_EXIST,
		M_NOUSER,
		M_INIT,
		M_EXIT,
		M_GETDATA,
		M_NODATA,
		M_TEXT,
		M_CONFIRM,
		M_INCORRECT,
		M_ACTIVE,
		M_INACTIVE,
		M_ABSENT
	}

	public struct MsgHeader
    {
		public string m_To;
		public string m_From;
		public int m_Type;
		public int m_Size;
    }
	class Message
    {
		private MsgHeader m_Header;
		private string m_Data;

		[DllImport("G:\\универ\\4 курс\\Леонов\\Lab2_Con2\\ClientServer2_2\\Debug\\SerializeLib.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
		private static extern void getM_HeaderFromString(string buffer, StringBuilder from, StringBuilder to, ref int type, ref int size);

		[DllImport("G:\\универ\\4 курс\\Леонов\\Lab2_Con2\\ClientServer2_2\\Debug\\SerializeLib.dll", SetLastError = true, CallingConvention = CallingConvention.Cdecl)]
		private static extern void  getSerializeString(StringBuilder str, string from, string to, int type, int size);

		public MsgHeader GetM_Header()
		{
			return m_Header;
		}
		public string getM_Data()
		{
			return m_Data;
		}
		public void setM_Data(string d)
		{
			m_Data = d;
		}

		public Message()
		{
            m_Header.m_To = "";
			m_Header.m_From = "";
			m_Header.m_Size = -1;
			m_Header.m_Type = -1;
		}

		public Message(string To, string From, Messages Type = Messages.M_TEXT, string Data = "")
		{
			m_Header = new MsgHeader();
			m_Header.m_To = To;
			m_Header.m_From = From;
			m_Header.m_Type = (int)Type;
			m_Data = Data;
			m_Header.m_Size = Data.Length;
		}

		public void Send(Socket s, string Password)
		{
            //ТЕСТОВЫЙ КОД ТЕСТИРУЕМ СЕРИАЛИЗАЦИЮ
            //var bufferSB = new StringBuilder(1024);
            //getSerializeString(bufferSB, m_Header.m_From, m_Header.m_To, m_Header.m_Type, m_Header.m_Size);
            //string buffer = bufferSB.ToString();

            //m_Header = new MsgHeader();
            //var FromSb = new StringBuilder(1024);
            //var ToSb = new StringBuilder(1024);
            //getM_HeaderFromString(bufferSB, FromSb, ToSb, ref m_Header.m_Type, ref  m_Header.m_Size);
            //m_Header.m_To = ToSb.ToString();
            //m_Header.m_From = FromSb.ToString();

            var bufferSB = new StringBuilder(1024);
			//string buffer=new string("");
            getSerializeString(bufferSB, m_Header.m_From, m_Header.m_To, m_Header.m_Type, m_Header.m_Size);
            string buffer = bufferSB.ToString();

            int len = buffer.Length;
            s.Send(BitConverter.GetBytes(len), sizeof(int), SocketFlags.None);
            s.Send(Encoding.UTF8.GetBytes(buffer), len, SocketFlags.None);


			if (m_Header.m_Type == (int)Messages.M_TEXT)
            {
                s.Send(Encoding.UTF8.GetBytes(m_Data), m_Header.m_Size, SocketFlags.None);
            }

			if (m_Header.m_Type == (int)Messages.M_INIT || m_Header.m_Type == (int)Messages.M_CREATE)
			{
				s.Send(BitConverter.GetBytes(Password.Length), sizeof(int), SocketFlags.None);
				s.Send(Encoding.UTF8.GetBytes(Password), Password.Length, SocketFlags.None);
			}

			
			
		}

		public MsgHeader Receive(Socket s)
		{
			byte[] b = new byte[4];
			s.Receive(b, sizeof(int), SocketFlags.None);
			int bufferSize = BitConverter.ToInt32(b, 0);
			b = new byte[bufferSize];
			s.Receive(b, bufferSize, SocketFlags.None);
			StringBuilder bufferSB = new StringBuilder();
			bufferSB.Append(Encoding.ASCII.GetString(b, 0, bufferSize));
            var FromSb = new StringBuilder(1024);
            var ToSb = new StringBuilder(1024);
            getM_HeaderFromString(bufferSB.ToString(), FromSb, ToSb, ref m_Header.m_Type, ref m_Header.m_Size);
            m_Header.m_To = ToSb.ToString();
            m_Header.m_From = FromSb.ToString();

            if (m_Header.m_Type == (int)Messages.M_TEXT)
            {
                b = new byte[m_Header.m_Size];
                s.Receive(b, m_Header.m_Size, SocketFlags.None);
				bufferSB = new StringBuilder();
				bufferSB.Append(Encoding.ASCII.GetString(b, 0, m_Header.m_Size));
                m_Data = bufferSB.ToString();
            }
			
			
            return m_Header;
		}

		public void SendMessage(Socket s, string To, string From, Messages Type = Messages.M_TEXT, string Data = "", string Password="")
		{
			Message msg = new Message(To, From, Type, Data);
			msg.Send(s, Password);
		}

	}
}
