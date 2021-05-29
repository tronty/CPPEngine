/*
 * This file is part of the iOSAppsFromScratch project.
 *
 * Copyright (C) 2010 Sean Nelson
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
#import "MainView.h"

@implementation MainView
- (void)loadView
{
	[super loadView];
	/*
	 * For this App, we're going to use a TabBar interface...
	 */
	NSMutableArray *localControllersArray = [[NSMutableArray alloc] initWithCapacity:3];

	/*
	 * Now we add our Tabs!
	 * Tab One Controller here!
	 */
	UITableViewController * tabOneController = [[UITableViewController alloc] init];
	UINavigationController * navCtrl = [[UINavigationController alloc] initWithRootViewController:tabOneController];
	tabOneController.title = @"TabOne";
	[tabOneController.tableView addSubview:navCtrl.navigationBar];
	[localControllersArray addObject:tabOneController];
	[tabOneController release];

	/*
	 * Tab Two Controller here!
	 */
	UITableViewController * tabTwoController = [[UITableViewController alloc] initWithStyle:UITableViewStyleGrouped];
	tabTwoController.title = @"TabTwo";
	[localControllersArray addObject:tabTwoController];
	[tabTwoController release];

	/*
	 * Tab Two Controller here!
	 */
	UITableViewController * tabThreeController = [[UITableViewController alloc] init];
	tabThreeController.title = @"TabThree";
	[localControllersArray addObject:tabThreeController];
	[tabThreeController release];

	self.viewControllers = localControllersArray;
}

- (void)viewDidLoad
{
	[super viewDidLoad];
}

- (void) viewDidUnload
{
	[self release];
}

- (void) didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];
}

- (void) dealloc
{
	[super dealloc];
}

@end
