/*
 *  Copyright © 2019 James Larrowe
 *
 *  This file is part of bf.
 *
 *  bf is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  bf is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with bf.  If not, see <https://www.gnu.org/licenses/>.
 */

package main

import (
   "os"
   "fmt"
   "bufio"
   "io/ioutil"
)

const default_slice_size = 30000
var slice_size = default_slice_size

func main() {
   sp := make([]byte, slice_size)

   if len(os.Args) > 1 {
      name := os.Args[1]

      ip, err := ioutil.ReadFile(name)

      if err != nil {
         fmt.Fprintln(os.Stderr, err)
         return
      }

      bf_interp(sp, ip)
   } else {
      fmt.Fprintln(os.Stderr, "Error: Incorrect number of arguments")
      fmt.Fprintln(os.Stderr, "  Usage:", os.Args[0], "[file] (args...)")
   }
}

func bf_interp(sp, ip []byte) {
   var spc, ipc int

   stdout := bufio.NewWriter(os.Stdout)
   stdin  := bufio.NewReader(os.Stdin)
   defer stdout.Flush()

   for ipc < len(ip) {
      switch ip[ipc] {
         case '>':
            spc++
            if spc >= slice_size {
               sp = append(sp, make([]byte, default_slice_size)...)
               slice_size += default_slice_size
            }
         case '<':
            spc--
         case '+':
            sp[spc]++
         case '-':
            sp[spc]--
         case '.':
            err := stdout.WriteByte(sp[spc])
            if err != nil {
               fmt.Fprintln(os.Stderr, err)
               return
            }
         case ',':
            stdout.Flush()
            c, err := stdin.ReadByte()
            if err == nil {
               sp[spc] = c
            }
         case '[':
            if sp[spc] == 0 {
               ipc++
               for nest := 1; nest != 0; ipc++ {
                  if ip[ipc] == '[' {
                     nest++
                  } else if ip[ipc] == ']' {
                     nest--
                  }
               }
            }
         case ']':
            if sp[spc] != 0 {
               ipc--
               for nest := 1; nest != 0; ipc-- {
                  if ip[ipc] == '[' {
                     nest--
                  } else if ip[ipc] == ']' {
                     nest++
                  }
               }
            }
         default:
            break
      }
      ipc++
   }
}
