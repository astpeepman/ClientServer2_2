using System;
using System.IO;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.Threading;


namespace SharpSlient
{
    class Program
    {
        public static string Login;
        public static IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), 11111);
        public static Mutex hMutex = new Mutex();
        static void canalStart(Socket s)
        {
            s.Connect(endPoint);
        }
        static void canalStop(Socket s)
        {
            s.Shutdown(SocketShutdown.Both);
            s.Close();
        }

        static void getData()
        {

            while (true)
            {
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Message msg = new Message();
                canalStart(s);
                msg.SendMessage(s, "@SERVER", Login, Messages.M_GETDATA);
                MsgHeader msgHeader = msg.Receive(s);
                canalStop(s);
                if (msgHeader.m_Type == (int)Messages.M_TEXT)
                {
                    Console.Write("Message from client " + msgHeader.m_From + ": " + msg.getM_Data()+'\n');
                }
                Thread.Sleep(2000);
            }
        }

        static bool ConnectToServer(Message msg, MsgHeader msgHeader)
        {
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            Console.WriteLine("0. Create Account\01. Use Account");
            int choice = Convert.ToInt32(Console.ReadLine());
            Console.WriteLine("Enter your login");
            Login = Console.ReadLine().ToString();
            Console.WriteLine("Enter your password");
            string Password = Console.ReadLine().ToString();
            switch (choice)
            {
                case 0:
                    {
                        canalStart(s);
                        msg.SendMessage(s, "@SERVER", Login, Messages.M_CREATE, "", Password);
                        break;
                    }
                case 1:
                    {
                        canalStart(s);
                        msg.SendMessage(s, "@SERVER", Login, Messages.M_INIT, "", Password);
                        break;
                    }
            }
            
            msgHeader = msg.Receive(s);
            canalStop(s);
            //Console.WriteLine(msgHeader.m_Type);
            if (msgHeader.m_Type == (int)Messages.M_CONFIRM)
            {
                hMutex.WaitOne();
                Console.Write("You have successfully connected to the server");
                hMutex.ReleaseMutex();
                Thread getDataThread = new Thread(getData);
                getDataThread.Start();
                return true;
            }
            //else if (msgHeader.m_Type == (int)Messages.M_TEXT)
            //{
            //    hMutex.WaitOne();
            //    Console.Write("You have successfully connected to the server\n", "Message from client " + msgHeader.m_From + ": " + msg.getM_Data());
            //    hMutex.ReleaseMutex();
            //    Thread getDataThread = new Thread(getData);
            //    getDataThread.Start();
            //    return true;
            //}
            else if (msgHeader.m_Type == (int)Messages.M_INCORRECT)
            {
                hMutex.WaitOne();
                Console.WriteLine("Sorry, wrong password");
                hMutex.ReleaseMutex();
                return false;
            }
            else if (msgHeader.m_Type == (int)Messages.M_ACTIVE)
            {
                hMutex.WaitOne();
                Console.WriteLine("Sorry, this user is already connected. " +
                    "You cannot run one account on different clients");
                hMutex.ReleaseMutex();
                return false;
            }
            else if (msgHeader.m_Type == (int)Messages.M_EXIST)
            {
                hMutex.WaitOne();
                Console.WriteLine("Sorry, this user already exists");
                hMutex.ReleaseMutex();
                return false;
            }
            else if (msgHeader.m_Type == (int)Messages.M_NOUSER)
            {
                hMutex.WaitOne();
                Console.WriteLine("Sorry, no such user was found.");
                hMutex.ReleaseMutex();
                return false;
            }
            else
            {
                hMutex.WaitOne();
                Console.WriteLine("Something went wrong");
                hMutex.ReleaseMutex();
                return false;
            }
        }
        static void Main(string[] args)
        {
            Message msg = new Message();
            MsgHeader msgHeader = new MsgHeader();
            int answer;
            bool flag = true;

            while (flag)
            {
                Console.WriteLine("Want to connect to a server? (0/1)");
                answer = Convert.ToInt32(Console.ReadLine());
                switch (answer)
                {
                    case 0: return;
                    case 1:
                        {
                            

                            if (!ConnectToServer(msg, msgHeader))
                            {
                                Console.WriteLine("Want to try some more? (0/1)");
                                answer = Convert.ToInt32(Console.ReadLine());
                                if (answer == 0)
                                {
                                    return;
                                }

                            }
                            else
                            {
                                flag = false;
                            }

                            break;
                        }
                    default:
                        {
                            Console.WriteLine("Please, press 0 or 1");
                            break;
                        }
                }
            }

            while (true)
            {
                Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
                Console.WriteLine("Press 0 for send Message\nPress 1 for Exit");
                answer = Convert.ToInt32(Console.ReadLine());
                switch (answer)
                {
                    case 0:
                        {
                            Console.WriteLine("Enter Login of client");
                            string ToLogin = Console.ReadLine().ToString();
                            Console.WriteLine("Enter your Message");
                            string str = Console.ReadLine().ToString();
                            canalStart(s);
                            msg.SendMessage(s, ToLogin, Login, Messages.M_TEXT, str);
                            msgHeader = msg.Receive(s);
                            canalStop(s);
                            hMutex.WaitOne();
                            if (msgHeader.m_Type == (int)Messages.M_INACTIVE)
                            {
                                Console.WriteLine("Your message will be delivered " +
                                    "as soon as the user connects");
                            }
                            else if (msgHeader.m_Type == (int)Messages.M_CONFIRM)
                            {
                                Console.WriteLine("The message was delivered successfully");
                            }
                            else if (msgHeader.m_Type == (int)Messages.M_ABSENT)
                            {
                                Console.WriteLine("The user you want to send a message to is not listed");
                            }
                            else
                            {
                                Console.WriteLine("Something went wrong");
                            }
                            hMutex.ReleaseMutex();
                            break;
                        }
                    case 1:
                        {
                            canalStart(s);
                            msg.SendMessage(s, "@SERVER", Login, Messages.M_EXIT);
                            msgHeader = msg.Receive(s);
                            canalStop(s);
                            hMutex.WaitOne();
                            if (msgHeader.m_Type == (int)Messages.M_CONFIRM)
                            {
                                Console.WriteLine("You have successfully disconnected");
                                
                            }
                            else if (msgHeader.m_Type == (int)Messages.M_INACTIVE)
                            {
                                Console.WriteLine("You have been disconnected by the server");
                                
                            }
                            else Console.WriteLine("An error has occurred");
                            hMutex.ReleaseMutex();

                            Environment.Exit(0);
                            break;
                        }
                }
            }
        }
    }
}
