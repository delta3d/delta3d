/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author David Guthrie
 */
#include <algorithm>
#include <ctype.h>
#include <string>
#include <vector>
#include <functional>

namespace dtDAL {

    class IsSpace;

    template <class Pred=IsSpace>
    class StringTokenizer {
    public:
        //The predicate should evaluate to true when applied to a separator.
        static void tokenize(std::vector<std::string>& roResult, std::string const& rostr,
                            Pred const& roPred=Pred());
    };

    //The predicate should evaluate to true when applied to a separator.
    template <class Pred>
    inline void StringTokenizer<Pred>::tokenize(std::vector<std::string>& roResult,
                                                std::string const& rostr, Pred const& roPred) {
        //First clear the results std::vector
        roResult.clear();
        std::string::const_iterator it = rostr.begin();
        std::string::const_iterator itTokenEnd = rostr.begin();
        while(it != rostr.end()) {
            //Eat seperators
            while(roPred(*it))
            it++;
            //Find next token
            itTokenEnd = std::find_if(it, rostr.end(), roPred);
            //Append token to result
            if(it < itTokenEnd)
            roResult.push_back(std::string(it, itTokenEnd));
            it = itTokenEnd;
        }
    }


    class IsSpace : public std::unary_function<char, bool> {
    public:
        bool operator()(char c) const;
    };

    inline bool IsSpace::operator()(char c) const {
        // isspace<char> returns true if c is a white-space character
        // (0x09-0x0D or 0x20)
        return isspace(c);
    }

    /**
     * Trims whitespace off the front and end of a string
     * @param toTrim the string to trim.
     */
    inline void trim(std::string& toTrim) {
        for (std::string::iterator i = toTrim.begin(); i != toTrim.end(); ++i) {
            if (isspace(*i))
                i = toTrim.erase(i);
            else
                break;
        }

        for (int i = toTrim.size() - 1; i <= 0; --i) {
            if (isspace(toTrim[i]))
                //we can just erase from the end because
                //it will shorted the part of the string already covered by the loop.
                toTrim.erase(i);
            else
                break;
        }
    }


}
