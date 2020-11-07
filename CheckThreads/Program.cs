using System;

namespace CheckThreads
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length>0)
            {
                var threadChecker = new ThreadChecker(args[0]);
                threadChecker.Check();
            }
            else
                Console.WriteLine("Syntax: prg <pid> or <program name>");
        }
    }
}
