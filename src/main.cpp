/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sunhwang <sunhwang@student.42seoul.kr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/06/02 22:07:10 by sunhwang         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Master.hpp"

void leaks()
{
    system("leaks webserv");
}

int main(int argc, char const *argv[])
{
    // atexit(leaks);
    Master master(argc, argv);
    Worker worker = master.create();
    worker.run();
    return 0;
}
