/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yje <yje@student.42seoul.kr>               +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/14 09:57:38 by sunhwang          #+#    #+#             */
/*   Updated: 2023/05/02 15:25:47 by yje              ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Master.hpp"
#include "Worker.hpp"

int main(int, char const *[])
{
    Master master;
    Worker worker(master);
    worker.run();

    return 0;
}
