﻿using System;

namespace CheckThreads
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                if (args.Length > 0)
                {
                    ThreadInfo.DisplayState = true;
                    var threadChecker = new ThreadChecker(args[0]);
                    threadChecker.Check();

                }
                else
                    Console.WriteLine("Syntax: prg <pid> or <program name>");
            }
            catch (Exception e)
            {
                Console.WriteLine(e);
            }
        }
    }
}
