using System;
using System.Collections.Generic;
using System.Threading;
using System.Threading.Tasks;

namespace ExampleWithThread
{
    class Program
    {
        static void DoSomeWork()
        {
            Console.WriteLine("Starting task");
            for (int i = 0; i < 10000; i++)
            {
                double s = 0.0;
                for (double x = 0; x < 100.0; x += 0.01)
                    s += Math.Sin(x);
                Thread.Sleep(100);
            }
        }

        static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");

            List<Task> list = new List<Task>();
            for (int i=0;i<5;i++)
            {
                list.Add(Task.Run(() => DoSomeWork()));
            }
            Task.WaitAll(list.ToArray());
        }
    }
}
